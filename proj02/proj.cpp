#define _USE_MATH_DEFINES

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

#include "height.h"

// setting the number of threads:
#ifndef NUMT
#define NUMT 1
#endif

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES 10
#endif

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

int main() {
    #ifndef _OPENMP
    fprintf(stderr, "No OpenMP support!\n");
    return 1;
    #endif

    omp_set_num_threads(NUMT);  // set the number of threads to use in the for-loop:`

    // get ready to record the maximum performance and the probability:
    double maxPerformance = 0.;  // must be declared outside the NUMTRIES loop
    // the area of a single full-sized tile:
    double fullTileArea = (((XMAX - XMIN) / (float) (NUMNODES - 1)) * ((YMAX - YMIN) / (float) (NUMNODES - 1)));

    printf("Iteration,Threads,MegaNodes,MegaHeightsComputedPerSecond,MaxPerformance,Volume,MaxVolume\n");

    // looking for the maximum performance:
    for (int t = 0; t < NUMTRIES; t++) {
        double time0 = omp_get_wtime();

        double maxVolume = 0.0;
        double volume = 0.0;
        // sum up the weighted heights into the variable "volume"
        // using an OpenMP for loop and a reduction:
        #pragma omp parallel for collapse(2) default(none) shared(fullTileArea) reduction(+:volume)
        for (int iv = 0; iv < NUMNODES; iv++) {
            for (int iu = 0; iu < NUMNODES; iu++) {
                float factor;
                if ((iu == 0 && iv == 0) || (iu == NUMNODES-1 && iv == 0)
                    || (iu == 0 && iv == NUMNODES-1) || (iu == NUMNODES-1 && iv == NUMNODES-1)) {
                    factor = 0.5;
                } else if (iu == 0 || iv == 0 || iu == NUMNODES-1 || iv == NUMNODES-1) {
                    factor = 1.0;
                } else {
                    factor = 2.0;
                }
                volume += factor * Height(iu, iv) * fullTileArea;
            }
        }

        double time1 = omp_get_wtime();
        double megaHeightsComputedPerSecond = (double) (NUMNODES * NUMNODES) / (time1 - time0) / 1000000.;
        if (megaHeightsComputedPerSecond > maxPerformance)
            maxPerformance = megaHeightsComputedPerSecond;
        if (volume > maxVolume)
            maxVolume = volume;
        printf("%d,%d,%d,%.2lf,%.2lf,%.2lf,%.2lf\n",
                t + 1, NUMT, (NUMNODES*NUMNODES)/1000000, megaHeightsComputedPerSecond,
                maxPerformance, volume, maxVolume);
    }

    return 0;
}
