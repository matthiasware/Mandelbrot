#include "mandelbrot.h"
#include <immintrin.h>
#include <cassert>
#include <iostream>

void mandelbrot_AVX_1(int w, int h, int maxiter,
       double re_min, double re_max,
       double im_min, double im_max,
       int *map)
{
  assert(w % 4 == 0);
  double re_step_d = ((re_max - re_min) / (w - 1));
  double im_step_d = ((im_max - im_min) / (h - 1));

  __m256d re_step = _mm256_setr_pd(re_step_d, 2*re_step_d, 3*re_step_d, 4*re_step_d);
  __m256d im_step = _mm256_set1_pd(-1 * im_step_d);
  __m256d c_im = _mm256_set1_pd(im_max);
  __m256d c_re = _mm256_setr_pd(re_min + 0 * re_step_d,
                                re_min + 1 * re_step_d,
                                re_min + 2 * re_step_d,
                                re_min + 3 * re_step_d);

  __m256d z_re;
  __m256d z_im;
  __m256d z_re_sqr;
  __m256d z_im_sqr;
  __m256d z_abs;
  __m256d iterations = _mm256_set1_pd(0);
  __m256d one = _mm256_set1_pd(1);
  __m256d mask;
  __m256d threshold = _mm256_set1_pd(4);
  for(int y=0; y<h; y++)
  {
    for(int x=0; x<w; x+=4)
    {
      z_im = c_im;
      z_re = c_re;
      z_re_sqr = _mm256_mul_pd(z_re, z_re);
      z_im_sqr = _mm256_mul_pd(z_im, z_im);
      z_abs = _mm256_add_pd(z_re_sqr, z_im_sqr);
      for(int i=0; i<maxiter; i++)
      {
        z_im = _mm256_mul_pd(z_re, z_im);
        z_im = _mm256_add_pd(z_im, z_im);
        z_im = _mm256_add_pd(z_im, c_im);
        z_re = _mm256_sub_pd(z_re_sqr, z_im_sqr);
        z_re = _mm256_add_pd(z_re, c_re);
        z_re_sqr = _mm256_mul_pd(z_re, z_re);
        z_im_sqr = _mm256_mul_pd(z_im, z_im);
        z_abs = _mm256_add_pd(z_re_sqr, z_im_sqr);
        mask = _mm256_cmp_pd(z_abs, threshold, _CMP_LT_OS);
        iterations = _mm256_add_pd(iterations, _mm256_and_pd(mask, one));

        if (_mm256_testz_pd(mask, _mm256_set1_pd(-1)))
        {
          break;
        }


      }
      double *values = (double *)& iterations;
      for(int i=0; i<4; i++)
      {
        map[y*w + x + i] = values[i];
        std::cout << values[i] << std::endl;
      }
      c_re = _mm256_add_pd(c_re, re_step);
    }
    c_im = _mm256_add_pd(c_im, im_step);
  }
}

int calcMandelbrot_tmp(double c_re, double c_im, int maxiter)
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


int main() {
  int w = 400;
  int h = 100;
  int maxiter = 100;
  double re_min = -2;
  double re_max = 1;
  double im_min = -1.2;
  double im_max = 1.2;
  int *map = new int[w * h];
  // int *threadIDs = new int[w * h];
  // test();
  mandelbrot_AVX_1(w, h, maxiter, re_min, re_max,
                   im_min, im_max, map);

  toFile(w, h, maxiter, map, "test", true);
}