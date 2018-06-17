#ifndef KERNELS_H_
#define KERNELS_H_

void filter (unsigned char* &input_image, unsigned char* &output_image, int width, int height, unsigned char* &dev_input, unsigned char* &dev_output );
void getOutputFilter(unsigned char* &output_image, unsigned char* &dev_input, unsigned char* &dev_output, int width, int height);
#endif
