//
// Created by sudha on 6/1/20.
//

#include <iostream>
#include <fstream>

using namespace std;

#include "not_parallel.h"
#include "openmp_parallel.h"
#include "cpu_simd.h"
#include "openmp_simd.h"
#include "gpu_opencl.h"

#ifndef MODE
#define MODE 0
#endif

#ifndef SIGNAL
#define SIGNAL "signal.txt"
#endif

int size;
float *a;
float *sum;

void readSignal() {
    ifstream signalFile;

    // open file stream
    signalFile.open(SIGNAL);
    if (!signalFile) {
        cerr << "Unable to open " << SIGNAL << endl;
        exit(1); // exit the program
    }

    // read from file
    if (signalFile >> size) {
        a = new float[2 * size];
        sum = new float[size];

        for (int i = 0; (signalFile >> a[i]) && (i < size); ++i) {
            a[i + size] = a[i];
        }
    }

    // close the file stream
    signalFile.close();
}

int main() {
    // read the signal from file
    readSignal();

    // compute the results
#if MODE == 4
    const double timeTaken = openCLAutoCorrelation(a, sum, size);
    cout << "OpenCL," << LOCAL_SIZE;
#elif MODE == 3
    const double timeTaken = openmpSimdAutoCorrelation(a, sum, size);
    cout << "OpenMP-SIMD," << NUMT;
#elif MODE == 2
    const double timeTaken = cpuSimdAutoCorrelation(a, sum, size);
    cout << "SIMD,0";
#elif MODE == 1
    const double timeTaken = openMPAutoCorrelation(a, sum, size);
    cout << "OpenMP," << NUMT;
#else
    const double timeTaken = notParallAutoCorrelation(a, sum, size);
    cout << "None,0";
#endif
    const double performance = (size * size / timeTaken) / 1000000000.;
    cout << "," << performance;

    // print the sums
    for (auto i = 1; i < min(513, size); ++i) cout << "," << sum[i];
    cout << endl;

    // free the pointers
    delete[] a;
    delete[] sum;

    return 0;
}
