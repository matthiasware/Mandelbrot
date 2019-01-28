#ifndef MANDELBROT_H_
#define MANDELBROT_H_
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>
#include <immintrin.h>
#include <cassert>

// https://de.wikipedia.org/wiki/Portable_Anymap#Pixmap
void toFile(int w, int h, int maxiter,
	        int *map, std::string fileName,
	        bool transpose=false) {
  if(transpose)
  {
  	int tmp = w;
  	w = h;
  	h = tmp;
  }
  std::ofstream img(fileName + ".ppm");
  if (!img.is_open()) {
    std::cout << "Could not open the file";
    return;
  }
  img << "P3\n" << h << " " << w << " 255\n";
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      int val = (map[x * h + y] % 255);
      img << val << ' ' << val << ' ' << val << "\n";
    }
  }
  img.close();
}

inline __m128i _mm256_castpd_epi32(__m256d &x)
{
  __m256d add = _mm256_set1_pd(6755399441055744.0);
  x = _mm256_add_pd(x, add);
  __m256i offset = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
  __m256i res = (__m256i) x;
  res = _mm256_permutevar8x32_epi32(res, offset);
  __m128i ri = _mm256_extracti128_si256(res, 0);
  return ri;
}

inline __m256i _mm256_castpd_epi32_sh(__m256d &x)
{
  __m256d add = _mm256_set1_pd(6755399441055744.0);
  x = _mm256_add_pd(x, add);
  __m256i offset = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
  __m256i res = (__m256i) x;
  // res = _mm256_permutevar8x32_epi32(res, offset);
  // __m128i ri = _mm256_extracti128_si256(res, 0);
  return res; 
}

// QImage mandelbrot1(int w, int h, int maxiter,
// 		   double re_min, double re_max,
// 		   double im_min, double im_max)
// {
// 	QImage img(w, h, QImage::Format_RGB888);


// 	double ref = ((re_max - re_min) / (w - 1));
// 	double imf =  ((im_max - im_min) / (h - 1));
// 	for(int x=0; x<w; x++)
// 	{
// 		double c_re = re_min + x * ref;
// 		for(int y=0; y<h; y++)
// 		{
// 			double c_im = im_max - y * imf;
// 			double Z_re = c_re;
// 			double Z_im = c_im;
// 			int i;
// 			for(i=0; i<maxiter; i++)
// 			{
// 				if(Z_re*Z_re + Z_im*Z_im > 4)
// 				{
// 					break;
// 				}
// 				double Z_im2 = Z_im * Z_im;
// 				Z_im = 2 * Z_re * Z_im + c_im;
// 				Z_re = Z_re * Z_re - Z_im2 + c_re;
// 			}
// 			// img.setPixelColor(x, y, getColor(i));
// 		}
// 	}
// 	return img;
// }


int calcMandelbrot0(double c_re, double c_im, int maxiter)
{
	double Z_re = c_re;
	double Z_im = c_im;
	int i;
	for(i=0; i<maxiter; i++)
	{
		if(Z_re*Z_re + Z_im*Z_im > 4)
		{
			break;
		}
		double Z_im2 = Z_im * Z_im;
		Z_im = 2 * Z_re * Z_im + c_im;
		Z_re = Z_re * Z_re - Z_im2 + c_re;
	}
	return i;
}
int calcMandelbrot1(double c_re, double c_im, int maxiter)
{
	double z_re = c_re;
	double z_im = c_im;
	int i=0;
	while(z_re * z_re + z_im * z_im <= 4 &&  i < maxiter)
	{
		double tmp = z_re * z_re - z_im * z_im + c_re;
		z_im = 2 * z_re * z_im + c_im;
		z_re = tmp;
		i++;
	}
	return i;
}

int calcMandelbrot2(double c_re, double c_im, int maxiter)
{
	double z_re = c_re;
	double z_im = c_im;
	double z_re_sqr = z_re * z_re;
	double z_im_sqr = z_im * z_im;
	int i=0;
	while(z_re_sqr + z_im_sqr <= 4 && i < maxiter)
	{
		z_im = z_re * z_im;
		z_im += z_im;  // z_im = z_im * 2
		z_im += c_im;
		z_re = z_re_sqr - z_im_sqr + c_re;
		z_re_sqr = z_re * z_re;
		z_im_sqr = z_im * z_im;
		i++;
	}
	return i;
}

void mandelbrot0(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	double c_re = re_min;
	double c_im = im_max;
	for(int x=0; x<w; x++)
	{
		c_im = im_max;
		for(int y=0; y<h; y++)
		{
			int val = calcMandelbrot0(c_re, c_im, maxiter);
			// std::cout << x << " " << y << " "
			// 		  << c_re << " " << c_im << " "
			// 		  << val << std::endl;
			map[x*h + y] = val;
			c_im -= im_step;
		}
		c_re += re_step;
	}
}
void mandelbrot1(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	double c_re = re_min;
	double c_im = im_max;
	for(int x=0; x<w; x++)
	{
		c_im = im_max;
		for(int y=0; y<h; y++)
		{
			int val = calcMandelbrot1(c_re, c_im, maxiter);
			// std::cout << x << " " << y << " "
			// 		  << c_re << " " << c_im << " "
			// 		  << val << std::endl;
			map[x*h + y] = val;
			c_im -= im_step;
		}
		c_re += re_step;
	}
}
void mandelbrot2(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	double c_re = re_min;
	double c_im = im_max;
	for(int x=0; x<w; x++)
	{
		c_im = im_max;
		for(int y=0; y<h; y++)
		{
			int val = calcMandelbrot2(c_re, c_im, maxiter);
			// std::cout << x << " " << y << " "
			// 		  << c_re << " " << c_im << " "
			// 		  << val << std::endl;
			map[x*h + y] = val;
			c_im -= im_step;
		}
		c_re += re_step;
	}
}

void mandelbrot_OMP_col(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	#pragma omp parallel for schedule(dynamic, 1)
	for(int x=0; x<w; x++)
	{
		double c_re = re_min + x * re_step;
		for(int y=0; y<h; y++)
		{
			double c_im = im_min + y * im_step;
			map[x*h + y] = calcMandelbrot1(c_re, c_im, maxiter);
		}
	}
}
void mandelbrot_OMP_row(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	// #pragma omp parallel for ordered schedule(dynamic)
	// #pragma omp parallel for
	// #pragma omp parallel for num_threads(4)
	#pragma omp parallel for schedule(dynamic, 1)
	for(int y=0; y<h; y++)
	{
		double c_im = im_min + y * im_step;
		for(int x=0; x<w; x++)
		{
			double c_re = re_min + x * re_step;
			map[y*w + x] = calcMandelbrot1(c_re, c_im, maxiter);
		}
	}
}


void mandelbrot_OMP_test(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map, int *threadIDs)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	// #pragma omp parallel for ordered schedule(dynamic)
	// #pragma omp parallel for schedule(dynamic, 8)
	#pragma omp parallel for collapse(2)
	for(int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
			double c_re = re_min + x * re_step;
			int tid = omp_get_thread_num();
			double c_im = im_min + y * im_step;
			map[y*w + x] = calcMandelbrot1(c_re, c_im, maxiter);
			threadIDs[y*w + x] = (255/4)*tid;
		}
	}
}

/*
	No memory allignment yet,
	can only compute images for 4*x  width
*/
void mandelbrot_AVX_1(int w, int h, int maxiter,
       double re_min, double re_max,
       double im_min, double im_max,
       int *map)
{
  assert(w % 4 == 0);
  double re_step_d = ((re_max - re_min) / (w - 1));
  double im_step_d = ((im_max - im_min) / (h - 1));

  __m256d re_step = _mm256_set1_pd(re_step_d * 4);
  __m256d im_step = _mm256_set1_pd(im_step_d);

  __m256d one = _mm256_set1_pd(1);
  __m256d neg_one = _mm256_set1_pd(-1);
  __m256d threshold = _mm256_set1_pd(4);
  __m256d c_im = _mm256_set1_pd(im_max);
  for(int y=0; y<h; y++)
  {
    __m256d c_re = _mm256_setr_pd(re_min + 0 * re_step_d,
                                  re_min + 1 * re_step_d,
                                  re_min + 2 * re_step_d,
                                  re_min + 3 * re_step_d);
    for(int x=0; x<w; x+=4)
    {
      __m256d z_im = c_im;
      __m256d z_re = c_re;
      __m256d iterations = _mm256_set1_pd(0);
      for(int i=0; i<maxiter; i++)
      {
        __m256d z_re_2 = _mm256_mul_pd(z_re, z_re);
        __m256d z_im_2 = _mm256_mul_pd(z_im, z_im);
        __m256d z_abs2 = _mm256_add_pd(z_re_2, z_im_2);
        __m256d mask = _mm256_cmp_pd(z_abs2, threshold, _CMP_LT_OS);
        iterations = _mm256_add_pd(_mm256_and_pd(mask, one), iterations);
        if (_mm256_testz_pd(mask, neg_one))
          break;
        // z_im(n) -> z_im(n+1)
        z_im = _mm256_mul_pd(z_re, z_im);
        z_im = _mm256_add_pd(z_im, z_im);
        z_im = _mm256_add_pd(z_im, c_im);
        // z_re(n) -> z_re(n+1)
        z_re = _mm256_sub_pd(z_re_2, z_im_2);
        z_re = _mm256_add_pd(z_re, c_re);


      }
      double *values = (double *)& iterations;
      for(int i=0; i<4; i++)
      {
        map[y*w + x + i] =  values[i];
      }
      c_re = _mm256_add_pd(c_re, re_step);
      // return;
    }
    c_im = _mm256_sub_pd(c_im, im_step);
  }
}

void mandelbrot_AVX_2(int w, int h, int maxiter,
       double re_min, double re_max,
       double im_min, double im_max,
       int *map)
{
  double re_step_d = ((re_max - re_min) / (w - 1));
  double im_step_d = ((im_max - im_min) / (h - 1));

  __m256d re_step = _mm256_set1_pd(re_step_d * 4);
  __m256d im_step = _mm256_set1_pd(im_step_d);

  __m256d one = _mm256_set1_pd(1);
  __m256d neg_one = _mm256_set1_pd(-1);
  __m256d threshold = _mm256_set1_pd(4);
  __m256d c_im = _mm256_set1_pd(im_max);
  for(int y=0; y<h; y++)
  {
    __m256d c_re = _mm256_setr_pd(re_min + 0 * re_step_d,
                                  re_min + 1 * re_step_d,
                                  re_min + 2 * re_step_d,
                                  re_min + 3 * re_step_d);
    for(int x=0; x<w; x+=4)
    {
      __m256d z_im = c_im;
      __m256d z_re = c_re;
      __m256d iterations = _mm256_set1_pd(0);
      for(int i=0; i<maxiter; i++)
      {
        __m256d z_re_2 = _mm256_mul_pd(z_re, z_re);
        __m256d z_im_2 = _mm256_mul_pd(z_im, z_im);
        __m256d z_abs2 = _mm256_add_pd(z_re_2, z_im_2);
        __m256d mask = _mm256_cmp_pd(z_abs2, threshold, _CMP_LT_OS);
        iterations = _mm256_add_pd(_mm256_and_pd(mask, one), iterations);
        if (_mm256_testz_pd(mask, neg_one))
          break;
        // z_im(n) -> z_im(n+1)
        z_im = _mm256_mul_pd(z_re, z_im);
        z_im = _mm256_add_pd(z_im, z_im);
        z_im = _mm256_add_pd(z_im, c_im);
        // z_re(n) -> z_re(n+1)
        z_re = _mm256_sub_pd(z_re_2, z_im_2);
        z_re = _mm256_add_pd(z_re, c_re);


      }
      __m128i r2 = _mm256_castpd_epi32(iterations);
      _mm_storeu_si128((__m128i *)&map[y*w+x], r2);
      c_re = _mm256_add_pd(c_re, re_step);
    }
    c_im = _mm256_sub_pd(c_im, im_step);
  }
}

inline __m128i calcMandelbrot_AVX(__m256d c_re, __m256d c_im, int maxiter)
{
	__m256d z_im = c_im;
  __m256d z_re = c_re;
  __m256d one = _mm256_set1_pd(1);
  __m256d neg_one = _mm256_set1_pd(-1);
  __m256d threshold = _mm256_set1_pd(4);
  __m256d iterations = _mm256_set1_pd(0);
  for(int i=0; i<maxiter; i++)
  {
    __m256d z_re_2 = _mm256_mul_pd(z_re, z_re);
    __m256d z_im_2 = _mm256_mul_pd(z_im, z_im);
    __m256d z_abs2 = _mm256_add_pd(z_re_2, z_im_2);
    __m256d mask = _mm256_cmp_pd(z_abs2, threshold, _CMP_LT_OS);
    iterations = _mm256_add_pd(_mm256_and_pd(mask, one), iterations);
    if (_mm256_testz_pd(mask, neg_one))
      break;
    // z_im(n) -> z_im(n+1)
    z_im = _mm256_mul_pd(z_re, z_im);
    z_im = _mm256_add_pd(z_im, z_im);
    z_im = _mm256_add_pd(z_im, c_im);
    // z_re(n) -> z_re(n+1)
    z_re = _mm256_sub_pd(z_re_2, z_im_2);
    z_re = _mm256_add_pd(z_re, c_re);
  }
  __m128i r2 = _mm256_castpd_epi32(iterations);
  return r2;
}

void mandelbrot_AVX_2_OMP(
			 int w, int h, int maxiter,
       double re_min, double re_max,
       double im_min, double im_max,
       int *map)
{
  double re_step_d = ((re_max - re_min) / (w - 1));
  double im_step_d = ((im_max - im_min) / (h - 1));

  __m256d re_step = _mm256_set1_pd(re_step_d * 4);
  __m256d im_step = _mm256_set1_pd(im_step_d);
  __m256d c_re_init = _mm256_setr_pd(re_min,
                                     re_min + re_step_d,
                                     re_min + re_step_d + re_step_d,
                                     re_min + re_step_d + re_step_d + re_step_d);

  #pragma omp parallel for schedule(dynamic, 1)
  for(int y=0; y<h; y++)
  {
  	__m256d c_im = _mm256_set1_pd(im_max - y * im_step_d);
    __m256d c_re = c_re_init;
    for(int x=0; x<w; x+=4)
    {
      __m128i r2 = calcMandelbrot_AVX(c_re, c_im, maxiter);
      _mm_storeu_si128((__m128i *)&map[y*w+x], r2);
      c_re = _mm256_add_pd(c_re, re_step);
    }
  }
}

void mandelbrot_AVX_2_OMP_OPTIMIZED(
			 int w, int h, int maxiter,
       double re_min, double re_max,
       double im_min, double im_max,
       int *map)
{
  double re_step_d = ((re_max - re_min) / (w - 1));
  double im_step_d = ((im_max - im_min) / (h - 1));

  __m256d re_step = _mm256_set1_pd(re_step_d * 4);
  __m256d im_step = _mm256_set1_pd(im_step_d);
  __m256d c_re_init = _mm256_setr_pd(re_min,
                                     re_min + re_step_d,
                                     re_min + re_step_d + re_step_d,
                                     re_min + re_step_d + re_step_d + re_step_d);
	__m256d one = _mm256_set1_pd(1);
	__m256d neg_one = _mm256_set1_pd(-1);
	__m256d threshold = _mm256_set1_pd(4);
	__m256d iterations = _mm256_set1_pd(0);

  #pragma omp parallel for schedule(dynamic, 1)
  for(int y=0; y<h; y++)
  {
  	__m256d c_im = _mm256_set1_pd(im_max - y * im_step_d);
    __m256d c_re = c_re_init;
    for(int x=0; x<w; x+=4)
    {
    	__m256d z_im = c_im;
		  __m256d z_re = c_re;
		  for(int i=0; i<maxiter; i++)
		  {
		    __m256d z_re_2 = _mm256_mul_pd(z_re, z_re);
		    __m256d z_im_2 = _mm256_mul_pd(z_im, z_im);
		    __m256d z_abs2 = _mm256_add_pd(z_re_2, z_im_2);
		    __m256d mask = _mm256_cmp_pd(z_abs2, threshold, _CMP_LT_OS);
		    iterations = _mm256_add_pd(_mm256_and_pd(mask, one), iterations);
		    if (_mm256_testz_pd(mask, neg_one))
		      break;
		    // z_im(n) -> z_im(n+1)
		    z_im = _mm256_mul_pd(z_re, z_im);
		    z_im = _mm256_add_pd(z_im, z_im);
		    z_im = _mm256_add_pd(z_im, c_im);
		    // z_re(n) -> z_re(n+1)
		    z_re = _mm256_sub_pd(z_re_2, z_im_2);
		    z_re = _mm256_add_pd(z_re, c_re);
		  }
		  __m128i r2 = _mm256_castpd_epi32(iterations);
      _mm_storeu_si128((__m128i *)&map[y*w+x], r2);
      c_re = _mm256_add_pd(c_re, re_step);
    }
  }
}

#endif // MANDELBROT_H_