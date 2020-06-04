//
// Created by sudha on 6/1/20.
//

#ifndef CS575_CPU_SIMD_H
#define CS575_CPU_SIMD_H

#include <omp.h>

#include "simd_helper.h"


double cpuSimdAutoCorrelation(const float *a, float *sum, const int size) {
    const double time0 = omp_get_wtime();

    for (auto shift = 0; shift < size; shift++) {
        sum[shift] = simdMulSum(a, a + shift, size);
    }

    const double time1 = omp_get_wtime();
    return time1 - time0;
}

#endif //CS575_CPU_SIMD_H
