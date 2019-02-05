#include "mandelbrot.h"
#include <immintrin.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <cstring>
#include <stdlib.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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


int main(int argc, char** argv) {

  // default values
  int w = 3000;
  int h = 2400;
  int maxiter = 500;
  double re_min = -2;
  double re_max = 1;
  double im_min = -1.2;
  double im_max = 1.2;

  std::string name = "mandelbrot.png";

  std::string usage =  "Usage: ./bmcreator\n"
                       "\t --name <outfile>\n"
                       "\t --width <pixel_width>\n"
                       "\t --height <pixel_height>\n"
                       "\t --maxiter <maxiter>\n"
                       "\t --imin <minimum imaginary value>\n"
                       "\t --imax <maximum imaginary value>\n"
                       "\t --rmin <minimum real value>\n"
                       "\t --rmax <maximum real value>\n";
  if((argc -1)%2 != 0)
  {
    std::cout << usage;
    return 0;
  }
  for (int i = 1; i < argc; i+=2)
  {
    if(argv[i] == std::string("--width"))
    {
      w = atoi(argv[i+1]);
    }
    else if(argv[i] == std::string("--height"))
    {
      h = atoi(argv[i+1]);
    }
    else if(argv[i] == std::string("--name"))
    {
      name = std::string(argv[i+1]);
    }
    else if(argv[i] == std::string("--maxiter"))
    {
      maxiter = atoi(argv[i+1]);
    }
    else if(argv[i] == std::string("--imin"))
    {
      im_min = atof(argv[i+1]);
    }
    else if(argv[i] == std::string("--imax"))
    {
      im_max = atof(argv[i+1]);
    }
    else if(argv[i] == std::string("--rmin"))
    {
      re_min = atof(argv[i+1]);
    }
    else if(argv[i] == std::string("--rmax"))
    {
      re_max = atof(argv[i+1]);
    }
    else
    {
      std::cout << usage;
      return 0;
    }
  }
  if(w % 4 != 0)
  {
    std::cout << "<width> must be multiple of 4" << std::endl;
    return 0;
  }
  int* map = (int*)aligned_alloc(32, h*w * sizeof(int));
  mandelbrot_avx(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  colorMap_omp(w, h, maxiter, map, RGBFORM::ABGR);
  int r = stbi_write_png(name.c_str(), w, h, 4, (void *) map, 4*w);
  if(r == 0)
  {
    std::cout << "Could not write image" << std::endl;
  }
  free(map);
}