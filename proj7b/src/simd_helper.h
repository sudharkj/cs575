//
// Created by sudha on 6/3/20.
//

#ifndef CS575_SIMD_HELPER_H
#define CS575_SIMD_HELPER_H

#include <xmmintrin.h>

#define SSE_WIDTH 4


float simdMulSum(const float *a, const float *b, const int len) {
    float sum[4] = {0., 0., 0., 0.};
    int limit = (len / SSE_WIDTH) * SSE_WIDTH;
    register const float *pa = a;
    register const float *pb = b;

    __m128 ss = _mm_loadu_ps(&sum[0]);
    for (int i = 0; i < limit; i += SSE_WIDTH) {
        ss = _mm_add_ps(ss, _mm_mul_ps(_mm_loadu_ps(pa), _mm_loadu_ps(pb)));
        pa += SSE_WIDTH;
        pb += SSE_WIDTH;
    }
    _mm_storeu_ps(&sum[0], ss);

    for (int i = limit; i < len; i++) {
        sum[0] += a[i] * b[i];
    }

    return sum[0] + sum[1] + sum[2] + sum[3];
}


#endif //CS575_SIMD_HELPER_H
