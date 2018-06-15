#include <cuda.h>
#include <cuda_runtime.h>


void getError(cudaError_t err);

float* gaussianDistance(float sigma, const int fsize);

