#include "mandelbrot.h"
#include <benchmark/benchmark.h>
#include <iostream>

const int w = 1440;
const int h = 1080;
const int maxiter = 1000;
const double re_min = -2.5;
const double re_max = 1.5;
const double im_min = -1.5;
const double im_max = 1.5;

// MANDELBROT SET CALCULATIONS

static void BM_mandelbrot(benchmark::State &state) {
  int *map = new int[w * h];
  for (auto _ : state) {
    mandelbrot(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb2");
}
BENCHMARK(BM_mandelbrot)->Unit(benchmark::kMillisecond);

#if defined _OPENMP
static void BM_mandelbrot_omp(benchmark::State &state) {

  int *map = new int[w * h];
  for (auto _ : state) {
    mandelbrot_omp(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb_omp", true);
}
BENCHMARK(BM_mandelbrot_omp)->Unit(benchmark::kMillisecond);
#endif

#if defined __AVX2__
static void BM_mandelbrot_avx(benchmark::State &state)  {
  int *map = (int*)aligned_alloc(32, h*w * sizeof(int));
  for (auto _ : state) {
    mandelbrot_avx(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb_avx", true);
}
BENCHMARK(BM_mandelbrot_avx)->Unit(benchmark::kMillisecond);
#endif

#if defined __AVX2__ && defined _OPENMP
static void BM_mandelbrot_avx_omp(benchmark::State &state)  {
  int *map = (int*)aligned_alloc(32, h*w * sizeof(int));
  for (auto _ : state) {
    mandelbrot_avx_omp(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb_avx_omp", true);
}
BENCHMARK(BM_mandelbrot_avx_omp)->Unit(benchmark::kMillisecond);
#endif

// COLORING


static void BM_colorMap(benchmark::State &state)  {
  int *map = (int*)aligned_alloc(32, h*w * sizeof(int));
  MANDELBROT(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  for (auto _ : state) {
    colorMap(w, h, maxiter, map, RGBFORM::ARGB);
  }
  // toFile(w, h, maxiter, map, "mb_avx_omp", true);
}
BENCHMARK(BM_colorMap)->Unit(benchmark::kMillisecond);


#if defined _OPENMP
static void BM_colorMap_omp(benchmark::State &state)  {
  int *map = (int*)aligned_alloc(32, h*w * sizeof(int));
  MANDELBROT(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  for (auto _ : state) {
    colorMap_omp(w, h, maxiter, map, RGBFORM::ARGB);
  }
  // toFile(w, h, maxiter, map, "mb_avx_omp", true);
}
BENCHMARK(BM_colorMap_omp)->Unit(benchmark::kMillisecond);
#endif


BENCHMARK_MAIN();