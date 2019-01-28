#include "mandelbrot.h"
#include <immintrin.h>
#include <cassert>
#include <iostream>
#include <iomanip>
//  https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=AVX,AVX2&cats=Store&expand=3525

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

//  https://github.com/skeeto/mandel-simd/blob/master/mandel_avx.c
void mandelbrot_AVX_3(
       int w, int h, int maxiter,
       double re_min, double re_max,
       double im_min, double im_max,
       int *map)
{
  double re_step_d = ((re_max - re_min) / (w - 1));
  double im_step_d = ((im_max - im_min) / (h - 1));

  __m256d re_step = _mm256_set1_pd(re_step_d * 4);
  __m256d im_step = _mm256_set1_pd(im_step_d);

  #pragma omp parallel for schedule(dynamic, 1)
  for(int y=0; y<h; y++)
  {
    __m256d c_im = _mm256_set1_pd(im_max - y * im_step_d);
    __m256d c_re = _mm256_setr_pd(re_min + 0 * re_step_d,
                                  re_min + 1 * re_step_d,
                                  re_min + 2 * re_step_d,
                                  re_min + 3 * re_step_d);
    for(int x=0; x<w; x+=4)
    {
      __m128i r2 = calcMandelbrot_avx(c_re, c_im, maxiter);
      _mm_storeu_si128((__m128i *)&map[y*w+x], r2);
      c_re = _mm256_add_pd(c_re, re_step);
    }
    c_im = _mm256_sub_pd(c_im, im_step);
  }
}
int calcMandelbrot_tmp(double c_re, double c_im, int maxiter)
{
  double z_re = c_re;
  double z_im = c_im;
  double z_re_2 = z_re * z_re;
  double z_im_2 = z_im * z_im;
  int i=0;
  while(z_re_2 + z_im_2 <= 4 && i < maxiter)
  {
    z_im = z_re * z_im;
    z_im += z_im;  // z_im = z_im * 2
    z_im += c_im;
    z_re = z_re_2 - z_im_2 + c_re;
    z_re_2 = z_re * z_re;
    z_im_2 = z_im * z_im;
    i++;

  }
  return i;
}

int doule2int(double d)
{
  int i=0;
  d += 6755399441055744.0;
    return i = *((int *)(&d));
}

int main() {
  int w = 400;
  int h = 100;
  int maxiter = 100;
  double re_min = -2;
  double re_max = 1;
  double im_min = -1.5;
  double im_max = 1.5;
  // int *map1 = new int[w * h];
  // int *map2 = new int[w * h];
  int* map1 = (int*)aligned_alloc(32, h*w * sizeof(int));
  int* map2 = (int*)aligned_alloc(32, h*w * sizeof(int));
  int* map3 = (int*)aligned_alloc(32, h*w * sizeof(int));
  // int *threadIDs = new int[w * h];
  // test();
  mandelbrot_omp(w, h, maxiter, re_min, re_max, im_min, im_max, map1);
  mandelbrot_avx(w, h, maxiter, re_min, re_max, im_min, im_max, map2);
  mandelbrot_avx_omp(w, h, maxiter, re_min, re_max, im_min, im_max, map3);
  bool equals = true;
  for(int i=0; i<w*h; i+=1)
  {
    if (map1[i] != map3[i])
    {
      equals = false;
      std::cout << map1[i] << " != " << map3[i] << std::endl;
    }
  }
  std::cout << "Equals: " << std::boolalpha << equals << std::endl;

  toFile(w, h, maxiter, map1, "map1", true);
  toFile(w, h, maxiter, map2, "map2", true);
  toFile(w, h, maxiter, map3, "map3", true);


  double darr[2] = {3.3, 3.5};
  darr[0] += 6755399441055744.0;
  darr[1] += 6755399441055744.0;

  std::cout << darr[0] << " " << darr[1] << std::endl;

  int *ip = (int*) darr;
  for(int i=0; i<4; i++)
  {
    std::cout << *(ip+i) << std::endl;
  }
}