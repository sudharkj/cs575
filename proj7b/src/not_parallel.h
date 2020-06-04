//
// Created by sudha on 6/1/20.
//

#ifndef CS575_NOT_PARALLEL_H
#define CS575_NOT_PARALLEL_H

#include <omp.h>


double notParallAutoCorrelation(const float *a, float *sum, const int size) {
    const double time0 = omp_get_wtime();

    for (auto shift = 0; shift < size; shift++) {
        float temp = 0.;
        for (auto i = 0; i < size; i++) {
            temp += a[i] * a[i + shift];
        }
        sum[shift] = temp;
    }

    const double time1 = omp_get_wtime();
    return time1 - time0;
}

#endif //CS575_NOT_PARALLEL_H
