#include "kernels.h"
#include "gaussian.h"
#include <iostream>
#include <cmath>


__global__
void blur(unsigned char* input_image, unsigned char* output_image, int width, int height) {

    const unsigned int offset = blockIdx.x*blockDim.x + threadIdx.x;// numer pixela
    int x = offset % width;
    int y = offset/width;
    int fsize = 5; // Filter size
    if(offset < width*height) {

        float output_red = 0;
        float output_green = 0;
        float output_blue = 0;
        int hits = 0;
        for(int ox = -fsize; ox < fsize+1; ++ox) {
            for(int oy = -fsize; oy < fsize+1; ++oy) {
                if((x+ox) > -1 && (x+ox) < width && (y+oy) > -1 && (y+oy) < height) {
                    const int currentoffset = (offset+ox+oy*width)*3;
                    output_red += input_image[currentoffset]; 
                    output_green += input_image[currentoffset+1];
                    output_blue += input_image[currentoffset+2];
                    hits++;
                }
            }
        }
        output_image[offset*3] = output_red/hits;
        output_image[offset*3+1] = output_green/hits;
        output_image[offset*3+2] = output_blue/hits;
        }
}


void filter (unsigned char* input_image, unsigned char* output_image, int width, int height) {

    unsigned char* dev_input;
    unsigned char* dev_output;
    getError(cudaMalloc( (void**) &dev_input, width*height*3*sizeof(unsigned char)));
    getError(cudaMemcpy( dev_input, input_image, width*height*3*sizeof(unsigned char), cudaMemcpyHostToDevice ));
 
    getError(cudaMalloc( (void**) &dev_output, width*height*3*sizeof(unsigned char)));

    //dim3 blockDims(512,1,1);
    //dim3 gridDims((unsigned int) ceil((double)(width*height*3/blockDims.x)), 1, 1 );

    //--- run kernel ---
    dim3 numThreads = dim3(BLOCKSIZE_X, BLOCKSIZE_Y, 1);
    dim3 numBlocks = dim3(width / numThreads.x, height / numThreads.y);

    // First run the warmup kernel (which we'll use to get the GPU in the correct max power state
    blur<<<numBlocks, numThreads>>>(dev_input, dev_output, width, height); 
    cudaDeviceSynchronize();

    //--- Allocate CUDA events that we'll use for timing ---
    cudaEvent_t start, stop;
    checkCudaErrors(cudaEventCreate(&start));
    checkCudaErrors(cudaEventCreate(&stop));

    printf("Launching CUDA Kernel\n");

    //--- Record the start event ---
    checkCudaErrors(cudaEventRecord(start, NULL));

    blur<<<numBlocks, numThreads>>>(dev_input, dev_output, width, height);
    //--- Record the stop event ---
    checkCudaErrors(cudaEventRecord(stop, NULL));

    //--- Wait for the stop event to complete ---
    checkCudaErrors(cudaEventSynchronize(stop));

    //--- Check to make sure the kernel didn't fail ---
    getLastCudaError("Kernel execution failed");

    float msecTotal = 0.0f;
    checkCudaErrors(cudaEventElapsedTime(&msecTotal, start, stop));

    getError(cudaMemcpy(output_image, dev_output, width*height*3*sizeof(unsigned char), cudaMemcpyDeviceToHost ));

    getError(cudaFree(dev_input));
    getError(cudaFree(dev_output));

}

