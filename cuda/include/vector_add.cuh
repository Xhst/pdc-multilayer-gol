#ifndef VECTOR_ADD_H
#define VECTOR_ADD_H

#define N 10000

// Declare the kernel function
__global__ void vectorAdd(float *a, float *b, float *c, int n);

#endif // VECTOR_ADD_H
