#include <immintrin.h>
#include <cmath>
#include <functional>
#include <chrono>
#include <random>
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <sys/time.h>

static int
time(const std::function<void ()> &f) {
	struct timeval start,end,diff;
    f();
	gettimeofday(&start,NULL);
    f();
	gettimeofday(&end,NULL);
	timersub(&end,&start,&diff);
    return diff.tv_sec*1'000'000+diff.tv_usec;
}

const int N = 50'000'000;
alignas(32) float x_a[N], y_a[N], z_a[N], q_a[N];
alignas(32) float x_b[N], y_b[N], z_b[N], q_b[N];

int main() {
	std::cout << ">> running for " << N << " line segments" << std::endl;

	std::default_random_engine eng;
	std::uniform_real_distribution<float> dist(-100000,100000);
	for (int i = 0; i < N; i++) {
		x_a[i] = dist(eng); x_b[i] = dist(eng);
		y_a[i] = dist(eng); y_b[i] = dist(eng);
		z_a[i] = dist(eng); z_b[i] = dist(eng);
		q_a[i] = dist(eng); q_b[i] = dist(eng);
	}

	static float l_s[N];
	auto run_seq = [&]() {
		getpid();
		for (int i = 0; i < N; i++) {
			l_s[i] = sqrt(
				((x_a[i]-x_b[i]) * (x_a[i]-x_b[i])) +
				((y_a[i]-y_b[i]) * (y_a[i]-y_b[i])) +
				((z_a[i]-z_b[i]) * (z_a[i]-z_b[i])) +
				((q_a[i]-q_b[i]) * (q_a[i]-q_b[i]))
			);
		}
	};
	int seq_usec = time(run_seq);
	std::cout << "Sequential: " << seq_usec << " usec" << std::endl;

	alignas(32) static float l_v[N];
	auto run_vec = [&]() {
		for (int i = 0; i < N/8; i++) {
			__m256 ymm_xa = _mm256_load_ps(x_a + 8*i); 
			__m256 ymm_ya = _mm256_load_ps(y_a + 8*i); 
			__m256 ymm_za = _mm256_load_ps(z_a + 8*i); 
			__m256 ymm_qa = _mm256_load_ps(q_a + 8*i); 

			__m256 ymm_xb = _mm256_load_ps(x_b + 8*i);
			__m256 ymm_yb = _mm256_load_ps(y_b + 8*i);
			__m256 ymm_zb = _mm256_load_ps(z_b + 8*i);
			__m256 ymm_qb = _mm256_load_ps(q_b + 8*i);

// 			__m256 ymm_l = _mm256_sqrt_ps(
// 				_mm256_add_ps(
// 	 				_mm256_add_ps(
// 						_mm256_mul_ps( _mm256_sub_ps(ymm_xa,ymm_xb), _mm256_sub_ps(ymm_xa,ymm_xb)),
// 	 					_mm256_mul_ps( _mm256_sub_ps(ymm_ya,ymm_yb), _mm256_sub_ps(ymm_ya,ymm_yb))
// 					),
// 					_mm256_add_ps(
// 	 					_mm256_mul_ps( _mm256_sub_ps(ymm_za,ymm_zb), _mm256_sub_ps(ymm_za,ymm_zb)),
// 	 					_mm256_mul_ps( _mm256_sub_ps(ymm_qa,ymm_qb), _mm256_sub_ps(ymm_qa,ymm_qb))
// 					)
// 				)
// 			);

 			__m256 ymm_l = _mm256_sqrt_ps(
 				_mm256_mul_ps( (ymm_xa-ymm_xb), (ymm_xa-ymm_xb) ) +
 	 			_mm256_mul_ps( (ymm_ya-ymm_yb), (ymm_ya-ymm_yb) ) +
 	 			_mm256_mul_ps( (ymm_za-ymm_zb), (ymm_za-ymm_zb) ) +
 	 			_mm256_mul_ps( (ymm_qa-ymm_qb), (ymm_qa-ymm_qb) )
 			);
			_mm256_store_ps(l_v + 8*i, ymm_l);

		}
	};
	int vec_usec = time(run_vec);
	std::cout << "Vector: " << vec_usec << " usec" << std::endl;

	float max = std::numeric_limits<float>::min();
	for (int i = 0; i < N; i++) {
		float diff = (l_s[i]-l_v[i]);
		if (diff != 0) {
			fprintf(stderr,"err:\tl_s[i]-l_v[i] =\t%f-%f =\t%f\n",l_s[i],l_v[i],diff);
		}
		max = std::max(max, std::abs(diff));
	}
	if (max == std::numeric_limits<float>::min())
		std::cout << ">> no errors" << std::endl;
	else
		std::cout << ">> max absolute error: " << max << std::endl;

	std::cout << ">> vectorized is " << ((float)seq_usec/vec_usec) << " times faster" << std::endl;
	return 0;
}
