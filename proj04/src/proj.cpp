#define _USE_MATH_DEFINES

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <xmmintrin.h>

#define SSE_WIDTH 4
#define NUM_TRIES 10

// how many tries to discover the maximum performance:
#ifndef USESIMD
#define USESIMD 0
#endif

// how many tries to discover the maximum performance:
#ifndef USECORE
#define USECORE 0
#endif

// setting the number of threads:
#ifndef ARRAYSIZE
#define ARRAYSIZE 10000
#endif

#if USECORE
#define NUM_ELEMENTS_PER_CORE ARRAYSIZE / USECORE
#endif

float A[ARRAYSIZE];
float B[ARRAYSIZE];

float SimdMulSum(float *a, float *b, int len) {
    float sum[4] = {0., 0., 0., 0.};
    int limit = (len / SSE_WIDTH) * SSE_WIDTH;
    register float *pa = a;
    register float *pb = b;

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

int main() {
    #ifndef _OPENMP
    fprintf(stderr, "No OpenMP support!\n");
    return 1;
    #endif

    for (int i = 0; i < ARRAYSIZE; ++i) {
        A[i] = 1.;
        B[i] = 1.;
    }

    float sum = 0;
    #if USECORE
    omp_set_num_threads(USECORE);	// same as # of sections
    #endif

    double maxMegaMultsPerSecond = 0.;
    for (int t = 0; t < NUM_TRIES; t++) {
        sum = 0.;
        double time0, time1;

        time0 = omp_get_wtime();
        #if USECORE && USESIMD
        #pragma omp parallel default(none) shared(A, B) reduction(+:sum)
        {
            int first = omp_get_thread_num( ) * NUM_ELEMENTS_PER_CORE;
            sum += SimdMulSum(&A[first], &B[first], NUM_ELEMENTS_PER_CORE);
        }
        #elif USESIMD
        sum = SimdMulSum(A, B, ARRAYSIZE);
        #elif USECORE
        #pragma omp parallel for default(none) shared(A, B) reduction(+:sum)
        for(int i = 0; i < ARRAYSIZE; i += 1) {
            sum += A[i] * B[i];
        }
        #else
        for (int i = 0; i < ARRAYSIZE; i += 1) {
            sum += A[i] * B[i];
        }
        #endif
        time1 = omp_get_wtime();

        double megaMultsPerSecond = (float) ARRAYSIZE / (time1 - time0) / 1000000.;
        if (megaMultsPerSecond > maxMegaMultsPerSecond)
            maxMegaMultsPerSecond = megaMultsPerSecond;
    }
    printf("%d,%d,%d,%.2lf,%.2lf\n", USESIMD, USECORE, ARRAYSIZE, sum, maxMegaMultsPerSecond);

    return 0;
}
