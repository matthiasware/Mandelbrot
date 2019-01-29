#include "mandelbrot.h"
#include <immintrin.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <cstring>
//  https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=AVX,AVX2&cats=Store&expand=3525

void inspectc(__m256i &v)
{
  __m256i tmp = _mm256_or_si256(v, _mm256_set1_epi32(0xff000000));
  int *d = (int *) &tmp;
  std::cout << std::hex << d[0] << " " << d[1]  << " " << d[2] << " " << d[3] << " "
            << d[4] << " " << d[5]  << " " << d[6] << " " << d[7] 
            << std::endl;
}

int calcRed(double pc)
{
  return std::min(255, static_cast<int>(pc * 1275));
}

// __m256i calcRed(__m256 pcs)
// {
//   __m256 red = _mm256_set1_ps(1275);
//   red = _mm256_mul_ps(red, pcs);
//   __m256i red_int = _mm256_cvtps_epi32(red);
//   red_int = _mm256_slli_epi32(red_int, 16);
//   return red_int;
// }

int main() {
  int w = 40;
  int h = 10;
  int maxiter = 1000;
  double re_min = -2;
  double re_max = 1;
  double im_min = -1.5;
  double im_max = 1.5;

  int* map_work = (int*)aligned_alloc(32, h*w * sizeof(int));
  int* map_test = (int*)aligned_alloc(32, h*w * sizeof(int));
  mandelbrot_avx(w, h, maxiter, re_min, re_max, im_min, im_max, map_work);
  std::memcpy(map_test, map_work, w*h*sizeof(int));
  colorMap(w, h, maxiter, map_work);
  colorMap_avx(w, h, maxiter, map_test);

  __m256 pcs = _mm256_setr_ps(0.0, 0.1, 0.15, 0.2, 0.3, 0.4, 0.8, 1.0);
  __m256 red = _mm256_set1_ps(1275);
  red = _mm256_mul_ps(red, pcs);
  __m256i red_int = _mm256_cvtps_epi32(red);
  red_int = _mm256_slli_epi32(red_int, 16);
  // __m256i reds = red(pcs);
  inspect(red);
  inspectc(red_int);

  std::cout << std::hex << (0xff000000 | calcRed(0.0)<<16) << " "
            << std::hex << (0xff000000 | calcRed(0.1)<<16) << " "
            << std::hex << (0xff000000 | calcRed(0.15)<<16) << " "
            << std::hex << (0xff000000 | calcRed(0.2)<<16) << " "
            << std::hex << (0xff000000 | calcRed(0.3)<<16) << " "
            << std::hex << (0xff000000 | calcRed(0.4)<<16) << " "
            << std::hex << (0xff000000 | calcRed(0.8)<<16) << " "
            << std::hex << (0xff000000 | calcRed(1.0)<<16) << std::endl;
  // toF  ile(w, h , maxiter, map, "map", true);
  // toFileColor(w, h , maxiter, map, "map_color", true);
  // int col = 0xff000000;
  // int r = 210;
  // int g = 11;
  // int b = 129;

  // col = (col  | r<<16 | g<<8 | b);
  // std::cout << std::hex << col << std::endl;
  // for(int i=0; i<=100; i++)
  // {
  //   double pc = i / 100.0;
  //   std::cout << pc << ":    " << red(pc) << "    " << green(pc) << "    " << blue(pc) << std::endl;
  // }
}