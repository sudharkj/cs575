//
// Created by sudha on 4/16/20.
//

#ifndef CS575_HEIGHT_H
#define CS575_HEIGHT_H

#ifndef NUMNODES
#define NUMNODES 4
#endif

#ifndef N
#define N 4
#endif

float Height(int, int);

float Height(int iu, int iv) {
    // iu,iv = 0 .. NUMNODES-1
    float x = -1. + 2. * (float) iu / (float) (NUMNODES - 1);    // -1. to +1.
    float y = -1. + 2. * (float) iv / (float) (NUMNODES - 1);    // -1. to +1.

    float xn = pow(fabs(x), (double) N);
    float yn = pow(fabs(y), (double) N);
    float r = 1. - xn - yn;
    if (r < 0.)
        return 0.;
    float height = pow(1. - xn - yn, 1. / (float) N);
    return height;
}

#endif //CS575_HEIGHT_H
