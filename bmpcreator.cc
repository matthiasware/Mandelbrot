#include "mandelbrot.h"
#include <immintrin.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <cstring>
//  https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=AVX,AVX2&cats=Store&expand=3525

void inspect(__m256d &v)
{
  double *d = (double *) &v;
  std::cout << d[0] << " " << d[1]  << " " << d[2] << " " << d[3] << std::endl;
}

void inspect(__m256 &v)
{
  float *d = (float *) &v;
  std::cout << d[0] << " " << d[1]  << " " << d[2] << " " << d[3] << " "
            << d[4] << " " << d[5]  << " " << d[6] << " " << d[7] 
            << std::endl;
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

// w, h, maxiter, name
int main(int argc, char** argv) {

  for (int i = 0; i < argc; ++i) 
    std::cout << argv[i] << "\n"; 
  // int w = 40;
  // int h = 10;
  // int maxiter = 1000;
  // double re_min = -2;
  // double re_max = 1;
  // double im_min = -1.5;
  // double im_max = 1.5;

  // int* map_work = (int*)aligned_alloc(32, h*w * sizeof(int));
  // int* map_test = (int*)aligned_alloc(32, h*w * sizeof(int));
  // mandelbrot_avx(w, h, maxiter, re_min, re_max, im_min, im_max, map_work);
  // std::memcpy(map_test, map_work, w*h*sizeof(int));
  // colorMap(w, h, maxiter, map_work);
}