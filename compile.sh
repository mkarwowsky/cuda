#! /bin/bash

rm gaussian
rm kernels.o
nvcc -c kernels.cu
nvcc -ccbin g++ -Xcompiler "-std=c++11" kernels.o main.cpp lodepng.cpp gaussian.cpp -lcuda -lcudart -o gaussian
