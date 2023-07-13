/* Spa */
/* SPDX-FileCopyrightText: Copyright © 2019 Wim Taymans */
/* SPDX-License-Identifier: MIT */

#include "resample-native-impl.h"

#include <tmmintrin.h>

static inline void inner_product_ssse3(float *d, const float * SPA_RESTRICT s,
		const float * SPA_RESTRICT taps, uint32_t n_taps)
{
	__m128 sum = _mm_setzero_ps();
	__m128 t0, t1;
	uint32_t i;

	switch (SPA_PTR_ALIGNMENT(s, 16)) {
	case 0:
		for (i = 0; i < n_taps; i += 8) {
			sum = _mm_add_ps(sum,
				_mm_mul_ps(
					_mm_load_ps(s + i + 0),
					_mm_load_ps(taps + i + 0)));
			sum = _mm_add_ps(sum,
				_mm_mul_ps(
					_mm_load_ps(s + i + 4),
					_mm_load_ps(taps + i + 4)));
		}
		break;
	case 4:
		t0 = _mm_load_ps(s - 1);
		for (i = 0; i < n_taps; i += 8) {
			t1 = _mm_load_ps(s + i + 3);
			t0 = (__m128)_mm_alignr_epi8((__m128i)t1, (__m128i)t0, 4);
			sum = _mm_add_ps(sum,
				_mm_mul_ps(t0, _mm_load_ps(taps + i + 0)));
			t0 = t1;
			t1 = _mm_load_ps(s + i + 7);
			t0 = (__m128)_mm_alignr_epi8((__m128i)t1, (__m128i)t0, 4);
			sum = _mm_add_ps(sum,
				_mm_mul_ps(t0, _mm_load_ps(taps + i + 4)));
			t0 = t1;
		}
		break;
	case 8:
		t0 = _mm_load_ps(s - 2);
		for (i = 0; i < n_taps; i += 8) {
			t1 = _mm_load_ps(s + i + 2);
			t0 = (__m128)_mm_alignr_epi8((__m128i)t1, (__m128i)t0, 8);
			sum = _mm_add_ps(sum,
				_mm_mul_ps(t0, _mm_load_ps(taps + i + 0)));
			t0 = t1;
			t1 = _mm_load_ps(s + i + 6);
			t0 = (__m128)_mm_alignr_epi8((__m128i)t1, (__m128i)t0, 8);
			sum = _mm_add_ps(sum,
				_mm_mul_ps(t0, _mm_load_ps(taps + i + 4)));
			t0 = t1;
		}
		break;
	case 12:
		t0 = _mm_load_ps(s - 3);
		for (i = 0; i < n_taps; i += 8) {
			t1 = _mm_load_ps(s + i + 1);
			t0 = (__m128)_mm_alignr_epi8((__m128i)t1, (__m128i)t0, 12);
			sum = _mm_add_ps(sum,
				_mm_mul_ps(t0, _mm_load_ps(taps + i + 0)));
			t0 = t1;
			t1 = _mm_load_ps(s + i + 5);
			t0 = (__m128)_mm_alignr_epi8((__m128i)t1, (__m128i)t0, 12);
			sum = _mm_add_ps(sum,
				_mm_mul_ps(t0, _mm_load_ps(taps + i + 4)));
			t0 = t1;
		}
		break;
	}
	sum = _mm_add_ps(sum, _mm_movehdup_ps(sum));
	sum = _mm_add_ss(sum, _mm_movehl_ps(sum, sum));
	_mm_store_ss(d, sum);
}

static inline void inner_product_ip_ssse3(float *d, const float * SPA_RESTRICT s,
	const float * SPA_RESTRICT t0, const float * SPA_RESTRICT t1, float x,
	uint32_t n_taps)
{
	float sum[2] = { 0.0f, 0.0f };
	uint32_t i;

	for (i = 0; i < n_taps; i++) {
		sum[0] += s[i] * t0[i];
		sum[1] += s[i] * t1[i];
	}
	*d = (sum[1] - sum[0]) * x + sum[0];
}

MAKE_RESAMPLER_FULL(ssse3);
MAKE_RESAMPLER_INTER(ssse3);
