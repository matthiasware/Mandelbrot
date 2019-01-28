#include "mandelbrot.h"
#include <immintrin.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <math.h>
//  https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=AVX,AVX2&cats=Store&expand=3525


struct RGB
{
  int red;
  int green;
  int blue;
};

int red(double pc)
{
  return (int) std::min(255.0, pc * 255/0.2);
}
int green(double pc)
{
  return pc * 255.0;
}
int blue(double pc)
{
  return (int) (255/2 + 1)*sin(pc*3*3.141592653589793 - 1.5) + (255/2 + 1);
}
RGB calcColor(int i, int maxiter)
{
  double pc = ((double) i) / maxiter;
  int r = red(pc);
  int g = green(pc);
  int b = blue(pc);
  return RGB{r, g, b};
}

void toFileColor(int w, int h, int maxiter,
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
      int val = map[x * h + y];
      RGB rgb = calcColor(val, maxiter);
      int r = rgb.red;
      int g = rgb.green;
      int b =  rgb.blue;
      img << r << ' ' << g << ' ' << b << "\n";
    }
  }
  img.close();
}

void mandelbrot_col2(int w, int h, int maxiter,
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

int main() {
  int w = 30;
  int h = 10;
  int maxiter = 1000;
  double re_min = -2;
  double re_max = 1;
  double im_min = -1.5;
  double im_max = 1.5;

  int* map = (int*)aligned_alloc(32, h*w * sizeof(int));
  mandelbrot_avx(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  for(int i=0; i<w*h; i++)
  {
    int r=red(map[i]);
    int g=green(map[i]);
    int b=blue(map[i]);
    int col = 0xff000000 | r<<16 | g<<8 | b;
    map[i] = col;
    std::cout << std::hex << col << std::endl;
  }
  // toFile(w, h , maxiter, map, "map", true);
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