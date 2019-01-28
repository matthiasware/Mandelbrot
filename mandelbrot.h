#ifndef MANDELBROT_H_
#define MANDELBROT_H_
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>
#include <immintrin.h>
#include <cassert>
#include <math.h>

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

inline int calcMandelbrot(double c_re, double c_im, int maxiter)
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

void mandelbrot_col(int w, int h, int maxiter,
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
			int val = calcMandelbrot(c_re, c_im, maxiter);
			val += 1 - log(log(sqrt(c_re * c_re + c_im * c_im))) / log(2);
			map[x*h + y] = val;
			c_im -= im_step;
		}
		c_re += re_step;
	}
}

void mandelbrot(int w, int h, int maxiter,
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
			int val = calcMandelbrot(c_re, c_im, maxiter);
			map[x*h + y] = val;
			c_im -= im_step;
		}
		c_re += re_step;
	}
}

void mandelbrot_omp(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	#pragma omp parallel for schedule(dynamic, 1)
	for(int y=0; y<h; y++)
	{
		double c_im = im_min + y * im_step;
		for(int x=0; x<w; x++)
		{
			double c_re = re_min + x * re_step;
			map[y*w + x] = calcMandelbrot(c_re, c_im, maxiter);
		}
	}
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
  __m256i res = (__m256i) x;
  return res; 
}

inline __m128i calcMandelbrot_avx(__m256d c_re, __m256d c_im, int maxiter)
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

void mandelbrot_avx(
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

  for(int y=0; y<h; y++)
  {
  	__m256d c_im = _mm256_set1_pd(im_max - y * im_step_d);
    __m256d c_re = c_re_init;
    for(int x=0; x<w; x+=4)
    {
      __m128i r2 = calcMandelbrot_avx(c_re, c_im, maxiter);
      _mm_storeu_si128((__m128i *)&map[y*w+x], r2);
      c_re = _mm256_add_pd(c_re, re_step);
    }
  }
}

int doule2int(double d)
{
  int i=0;
  d += 6755399441055744.0;
    return i = *((int *)(&d));
}

void inspect(__m256d &v)
{
  double *d = (double *) &v;
  std::cout << d[0] << " " << d[1]  << " " << d[2] << " " << d[3] << std::endl;
}

void inspect(__m256i &v)
{
  int *d = (int *) &v;
  std::cout << d[0] << " " << d[1]  << " " << d[2] << " " << d[3] << " "
            << d[4] << " " << d[5]  << " " << d[6] << " " << d[7] 
            << std::endl;
}

void inspect(__m128i &v)
{
  int *d = (int *) &v;
  std::cout << d[0] << " " << d[1]  << " " << d[2] << " " << d[3] << std::endl;
}

void mandelbrot_avx_omp(
			 int w, int h, int maxiter,
       double re_min, double re_max,
       double im_min, double im_max,
       int *map)
{
  double re_step_d = ((re_max - re_min) / (w - 1));
  double im_step_d = ((im_max - im_min) / (h - 1));

  __m256i permutation_offset = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
  __m256d magic_offset = _mm256_set1_pd(6755399441055744.0);
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
	  __m256d one = _mm256_set1_pd(1);
	  __m256d neg_one = _mm256_set1_pd(-1);
	  __m256d threshold = _mm256_set1_pd(4);
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
		  __m256i res = (__m256i) _mm256_add_pd(iterations, magic_offset);
		  res = _mm256_permutevar8x32_epi32(res, permutation_offset);
		  __m128i r2 = _mm256_extracti128_si256(res, 0);

      _mm_storeu_si128((__m128i *)&map[y*w+x], r2);
      c_re = _mm256_add_pd(c_re, re_step);
    }
  }
}

#endif // MANDELBROT_H_