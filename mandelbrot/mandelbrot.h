#ifndef MANDELBROT_H_
#define MANDELBROT_H_

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <math.h>
#include <cstdint>

#if defined _OPENMP
  #include <omp.h>
#endif

#if defined __AVX2__
  #include <immintrin.h>
#endif

#ifndef MANDELBROT
  #if defined __AVX2__ && _OPENMP
    #define MANDELBROT mandelbrot_avx_omp
  #elif defined __AVX2__
    #define MANDELBROT mandelbrot_avx
  #elif defined _OPENMP
    #define MANDELBROT mandelbrot_omp
  #else
    #define MANDELBROT mandelbrot
  #endif
#endif

#ifndef COLOR
  #if defined _OPENMP
    #define COLORMAP colorMap_omp
  #else
    #define COLORMAP colorMap
  #endif
#endif

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

/*
see Pixmap:
https://de.wikipedia.org/wiki/Portable_Anymap#Pixmap
*/
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

/*
================= COLORING =================
Requirements:
	black: 0   iterations
	white: max iterations
	anything in between should be assigned a unique color

Giveen is 'n', the number of iterations needed:
We want a percentage for simpler color mapping:
pc = n / maxiter

red   =  min(255, (255/0.2)*pc) => peaks at 0.3 at max value 255
green =  pc * 255.0 => steadily increasing
blue  =  127*(sin(3pi*pc + pi/2) + 1)

#define CACHE_LINE_SIZE sysconf(_SC_LEVEL1_DCACHE_LINESIZE) 

*/

struct RGBFORM 
{
  static const uint8_t RGBA = 0b11'10'01'00;
  static const uint8_t ARGB = 0b10'01'00'11;
  static const uint8_t BGRA = 0b01'10'11'00;
  static const uint8_t ABGR = 0b00'01'10'11;
};

inline int formatColor(int r, int g, int b, int a, uint8_t mask)
{
  r = r << (((mask & 0b11000000)>>6)*8);
  g = g << (((mask & 0b00110000)>>4)*8);
  b = b << (((mask & 0b00001100)>>2)*8);
  a = a << (((mask & 0b00000011)>>0)*8);
  return (r | g | b | a);
}
int colorMap(const int w, const int h, const int maxiter, int *map, uint8_t mask)
{
  for(int i=0; i<w*h; i++)
  {
    int *val = &map[i];
    double pc = ((double) *val) / maxiter;

    int b = std::min(255, static_cast<int>(pc * 1275));
    int g = static_cast<int>(pc * 255.0);
    int r = static_cast<int>(127.0*(sin( -M_PI*3*pc  -M_PI * .5) + 1));
    // *val =  0xff000000 | r<<16 | g<<8 | b;
    *val = formatColor(r, g, b, 0xff, mask);
  }
}

#if defined _OPENMP
int colorMap_omp(const int w, const int h, const int maxiter, int *map, uint8_t mask)
{
	#pragma omp parallel for schedule(static)
  for(int i=0; i<w*h; i++)
  {
  	int *val = &map[i];
    double pc = ((double) *val) / maxiter;

    int b = std::min(255, static_cast<int>(pc * 1275));
    int g = static_cast<int>(pc * 255.0);
    int r = static_cast<int>(127.0*(sin( -M_PI*3*pc  -M_PI * .5) + 1));
    // *val = 0xff000000 | r<<16 | g<<8 | b;
    *val = formatColor(r, g, b, 0xff, mask);;
  }
}
#endif

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

#if defined _OPENMP
void mandelbrot_omp(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	#pragma omp parallel for schedule(dynamic)
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
#endif

#if defined __AVX2__
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
#endif

#if defined _OPENMP && defined __AVX2__
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

  #pragma omp parallel for schedule(dynamic)
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
#endif
#endif // MANDELBROT_H_