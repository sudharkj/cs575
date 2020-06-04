//
// Created by sudha on 6/1/20.
//

#ifndef CS575_OPENMP_PARALLEL_H
#define CS575_OPENMP_PARALLEL_H

#include <iostream>
#include <omp.h>

using namespace std;

// setting the number of threads:
#ifndef NUMT
#define NUMT 32
#endif


double openMPAutoCorrelation(const float *a, float *sum, const int size) {
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
        float temp = 0.;
        for (int i = 0; i < size; i++) {
            temp += a[i] * a[i + shift];
        }
        sum[shift] = temp;    // note the "fix #2" from false sharing if you are using OpenMP
    }

    const double time1 = omp_get_wtime();
    return time1 - time0;
}

#endif //CS575_OPENMP_PARALLEL_H
