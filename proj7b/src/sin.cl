kernel void AutoCorrelate(global const float *dA, global float *dSum) {
    int size = get_global_size(0);
    int shift = get_global_id(0);

    float sum = 0.;
    for (int i=0; i<size; ++i) {
        sum += dA[i] * dA[i + shift];
    }
    dSum[shift] = sum;
}
