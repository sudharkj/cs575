//
// Created by sudha on 6/3/20.
//

#ifndef CS575_OPENMP_SIMD_H
#define CS575_OPENMP_SIMD_H

#include <iostream>
#include <omp.h>

#include "simd_helper.h"

using namespace std;

// setting the number of threads:
#ifndef NUMT
#define NUMT 8
#endif


double openmpSimdAutoCorrelation(const float *a, float *sum, const int size) {
    // check for openmp support
#ifndef _OPENMP
    cerr << "No OpenMP support!" << endl;
    exit(1);
#endif

    // set the number of threads to use in the for-loop:`
    omp_set_num_threads(NUMT);

    const double time0 = omp_get_wtime();

#pragma omp parallel for default(none) shared(a, sum)
    for (auto shift = 0; shift < size; shift++) {
        sum[shift] = simdMulSum(a, a + shift, size);
    }

    const double time1 = omp_get_wtime();
    return time1 - time0;
}

#endif //CS575_OPENMP_SIMD_H
