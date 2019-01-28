#include "mandelbrot.h"
#include <benchmark/benchmark.h>

int w = 1440;
int h = 1080;
int maxiter = 256;
double re_min = -2.5;
double re_max = 1.5;
double im_min = -1.5;
double im_max = 1.5;

static void BM_calcMandelbrot_0(benchmark::State &state) {

  int *map = new int[w * h];
  for (auto _ : state) {
    mandelbrot0(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb0");
}

// BENCHMARK(BM_calcMandelbrot_0)->Unit(benchmark::kMillisecond);

static void BM_calcMandelbrot_1(benchmark::State &state) {

  int *map = new int[w * h];

  for (auto _ : state) {
    mandelbrot1(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb1");
}
// BENCHMARK(BM_calcMandelbrot_1)->Unit(benchmark::kMillisecond);

static void BM_calcMandelbrot_2(benchmark::State &state) {

  int *map = new int[w * h];
  for (auto _ : state) {
    mandelbrot2(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb2");
}
BENCHMARK(BM_calcMandelbrot_2)->Unit(benchmark::kMillisecond);

static void BM_calcMandelbrot_OMP_col(benchmark::State &state) {

  int *map = new int[w * h];
  for (auto _ : state) {
    mandelbrot_OMP_col(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb_omp_col");
}
// BENCHMARK(BM_calcMandelbrot_OMP_col)->Unit(benchmark::kMillisecond);

static void BM_calcMandelbrot_OMP_row(benchmark::State &state) {

  int *map = new int[w * h];
  for (auto _ : state) {
    mandelbrot_OMP_row(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb_omp_row", true);
}
BENCHMARK(BM_calcMandelbrot_OMP_row)->Unit(benchmark::kMillisecond);

static void BM_calcMandelbrot_AVX2_1(benchmark::State &state)  {
  int *map = new int[w * h];
  for (auto _ : state) {
    mandelbrot_AVX_1(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb_avx_1", true);
}
BENCHMARK(BM_calcMandelbrot_AVX2_1)->Unit(benchmark::kMillisecond);


static void BM_calcMandelbrot_AVX2_2(benchmark::State &state)  {
  int *map = (int*)aligned_alloc(32, h*w * sizeof(int));
  for (auto _ : state) {
    mandelbrot_AVX_2(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb_avx_2", true);
}
BENCHMARK(BM_calcMandelbrot_AVX2_2)->Unit(benchmark::kMillisecond);


static void BM_calcMandelbrot_AVX2_OMP(benchmark::State &state)  {
  int *map = (int*)aligned_alloc(32, h*w * sizeof(int));
  for (auto _ : state) {
    mandelbrot_AVX_2_OMP(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb_avx_OMP", true);
}
BENCHMARK(BM_calcMandelbrot_AVX2_OMP)->Unit(benchmark::kMillisecond);

static void BM_calcMandelbrot_AVX2_OMP_OPTIMIZED(benchmark::State &state)  {
  int *map = (int*)aligned_alloc(32, h*w * sizeof(int));
  for (auto _ : state) {
    mandelbrot_AVX_2_OMP_OPTIMIZED(w, h, maxiter, re_min, re_max, im_min, im_max, map);
  }
  toFile(w, h, maxiter, map, "mb_avx_OMP_OPTIMIZED", true);
}
BENCHMARK(BM_calcMandelbrot_AVX2_OMP_OPTIMIZED)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();