#include "mandelbrot.h"

int main() {
  int w = 1200;
  int h = 600;
  int maxiter = 10000;
  double re_min = -2;
  double re_max = 1;
  double im_min = -1.2;
  double im_max = 1.2;
  int *map = new int[w * h];
  int *threadIDs = new int[w * h];
  // test();
  mandelbrot_OMP_test(w, h, maxiter, re_min, re_max,
                   im_min, im_max, map, threadIDs);

  toFile(w, h, maxiter, threadIDs, "test", true);
}