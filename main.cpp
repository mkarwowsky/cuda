#include <iostream>
#include <cstdlib>
#include "lodepng.h"
#include <cuda.h>
#include <cuda_runtime.h>
#include "kernels.h"
#include <functional>

struct preparedImage {
    unsigned char* input_image;
    unsigned char* output_image;
};

preparedImage loadImage(char name_file){
    printf("Czytanie argumentów: %s %s \n", argv[1+j], argv[2+j]);
    std::vector<unsigned char> in_image;
    unsigned int width, height;

    // Load the data
    printf("Ładowanie danych \n");
    unsigned error = lodepng::decode(in_image, width, height, input_file);
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    
    // Prepare the data
    printf("Przygotowanie danych \n");
    preparedImage image;
    image.input_image = new unsigned char[(in_image.size()*3)/4];
    image.output_image = new unsigned char[(in_image.size()*3)/4];
    int where = 0;
    for(int i = 0; i < in_image.size(); ++i) {
       if((i+1) % 4 != 0) {
           image.input_image[where] = in_image.at(i);
           image.output_image[where] = 255;
           where++;
       }
    }
    return image;
}

int main(int argc, char** argv) {
    /*if(argc != 3) {
        std::cout << "Run with input and output image filenames." << std::endl;
        return 0;
    }*/

    for(int j = 0; j < argc; j+=2){

    // Read the arguments
    const char* input_file = argv[1+j];
    const char* output_file = argv[2+j];

    unsigned char* input_image, unsigned char* output_image
    preparedImage loadedImage;
    loadedImage = loadImage(input_file);
/*    printf("Czytanie argumentów: %s %s \n", argv[1+j], argv[2+j]);
    std::vector<unsigned char> in_image;
    unsigned int width, height;

    // Load the data
    printf("Ładowanie danych \n");
    unsigned error = lodepng::decode(in_image, width, height, input_file);
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    
    // Prepare the data
    printf("Przygotowanie danych \n");
    unsigned char* input_image = new unsigned char[(in_image.size()*3)/4];
    unsigned char* output_image = new unsigned char[(in_image.size()*3)/4];
    int where = 0;
    for(int i = 0; i < in_image.size(); ++i) {
       if((i+1) % 4 != 0) {
           input_image[where] = in_image.at(i);
           output_image[where] = 255;
           where++;
       }
    }
*/
    // Run the filter on it
    printf("Filter uruchom \n");
    filter(loadedImage.input_image, loadedImage.output_image, width, height); 

    // Prepare data for output
    printf("Dane na output \n");
    std::vector<unsigned char> out_image;
    for(int i = 0; i < in_image.size(); ++i) {
        out_image.push_back(loadedImage.output_image[i]);
        if((i+1) % 3 == 0) {
            out_image.push_back(255);
        }
    }
    
    // Output the data
    printf("Output data \n");
    error = lodepng::encode(output_file, out_image, width, height);
    
    //if there's an error, display it
    if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;

    delete[] input_image;
    delete[] output_image;
    }
    return 0;
    
}



