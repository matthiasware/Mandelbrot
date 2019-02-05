#ifndef MANDELBROT_AVX_OMP_H_
#define MANDELBROT_AVX_OMP_H_

void mandelbrot_avx_omp(
			 int w, int h, int maxiter,
       double re_min, double re_max,
       double im_min, double im_max,
       int *map)
{
  double re_step_d = ((re_max - re_min) / (w - 1));
  double im_step_d = ((im_max - im_min) / (h - 1));

  __m256i permutation_offset = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
  __m256d magic_offset = _mm256_set1_pd(6755399441055744.0);
  __m256d re_step = _mm256_set1_pd(re_step_d * 4);
  __m256d im_step = _mm256_set1_pd(im_step_d);
  __m256d c_re_init = _mm256_setr_pd(re_min,
                                     re_min + re_step_d,
                                     re_min + re_step_d + re_step_d,
                                     re_min + re_step_d + re_step_d + re_step_d);

  #pragma omp parallel for schedule(dynamic)
  for(int y=0; y<h; y++)
  {
  	__m256d c_im = _mm256_set1_pd(im_max - y * im_step_d);
    __m256d c_re = c_re_init;
	  __m256d one = _mm256_set1_pd(1);
	  __m256d neg_one = _mm256_set1_pd(-1);
	  __m256d threshold = _mm256_set1_pd(4);
    for(int x=0; x<w; x+=4)
    {
    	__m256d z_im = c_im;
		  __m256d z_re = c_re;
		  __m256d iterations = _mm256_set1_pd(0);
		  for(int i=0; i<maxiter; i++)
		  {
		    __m256d z_re_2 = _mm256_mul_pd(z_re, z_re);
		    __m256d z_im_2 = _mm256_mul_pd(z_im, z_im);
		    __m256d z_abs2 = _mm256_add_pd(z_re_2, z_im_2);
		    __m256d mask = _mm256_cmp_pd(z_abs2, threshold, _CMP_LT_OS);
		    iterations = _mm256_add_pd(_mm256_and_pd(mask, one), iterations);
		    if (_mm256_testz_pd(mask, neg_one))
		      break;
		    // z_im(n) -> z_im(n+1)
		    z_im = _mm256_mul_pd(z_re, z_im);
		    z_im = _mm256_add_pd(z_im, z_im);
		    z_im = _mm256_add_pd(z_im, c_im);
		    // z_re(n) -> z_re(n+1)
		    z_re = _mm256_sub_pd(z_re_2, z_im_2);
		    z_re = _mm256_add_pd(z_re, c_re);
		  }
		  __m256i res = (__m256i) _mm256_add_pd(iterations, magic_offset);
		  res = _mm256_permutevar8x32_epi32(res, permutation_offset);
		  __m128i r2 = _mm256_extracti128_si256(res, 0);

      _mm_storeu_si128((__m128i *)&map[y*w+x], r2);
      c_re = _mm256_add_pd(c_re, re_step);
    }
  }
}

#endif // MANDELBROT_AVX_OMP_H_