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


inline int formatColor1(int r, int g, int b, int a, uint8_t mask)
{
  r = r<< (((mask & 0b11000000)>>6)*8);
  g = g<< (((mask & 0b00110000)>>4)*8);
  b = b<< (((mask & 0b00001100)>>2)*8);
  a = a<< (((mask & 0b00000011))*8);
  return (r | g | b | a);
}

inline int formatColor2(int r, int g, int b, int a)
{
	return  a << 24 | r<<16 | g<<8 | b;
}

int main(int argc, char** argv) {

  // default values
  int w = 6000;
  int h = 4800;
  int maxiter = 2000;
  double re_min = -2;
  double re_max = 1;
  double im_min = -1.2;
  double im_max = 1.2;

  int r = 0xaa;
  int g = 0xbb;
  int b = 0xcc;
  int a = 0xdd;

  int val = formatColor1(r, g, b, a, RGBFORM::ARGB);
  std::cout << std::hex << val << std::endl;

  int val2 = formatColor2(r, g, b, a);
  std::cout << std::hex << val2 << std::endl;
}