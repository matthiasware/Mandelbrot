#include "mandelbrot.h"
#include <benchmark/benchmark.h>

int w = 2880;
int h = 2160;
int maxiter = 1000;
double re_min = -2.5;
double re_max = 1.5;
double im_min = -1.5;
double im_max = 1.5;

static void BM_mandelbrot(benchmark::State &state) {

  int *map = new int[w * h];
  for (auto _ : state) {
    mandelbrot(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb2");
}
BENCHMARK(BM_mandelbrot)->Unit(benchmark::kMillisecond);

static void BM_mandelbrot_omp(benchmark::State &state) {

  int *map = new int[w * h];
  for (auto _ : state) {
    mandelbrot_omp(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb_omp", true);
}
BENCHMARK(BM_mandelbrot_omp)->Unit(benchmark::kMillisecond);

static void BM_mandelbrot_avx(benchmark::State &state)  {
  int *map = (int*)aligned_alloc(32, h*w * sizeof(int));
  for (auto _ : state) {
    mandelbrot_avx(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb_avx", true);
}
BENCHMARK(BM_mandelbrot_avx)->Unit(benchmark::kMillisecond);

static void BM_mandelbrot_avx_omp(benchmark::State &state)  {
  int *map = (int*)aligned_alloc(32, h*w * sizeof(int));
  for (auto _ : state) {
    mandelbrot_avx_omp(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb_avx_omp", true);
}
BENCHMARK(BM_mandelbrot_avx_omp)->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN();