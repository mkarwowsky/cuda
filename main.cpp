#include <iostream>
#include <cstdlib>
#include "lodepng.h"
#include <cuda.h>
#include <cuda_runtime.h>
#include "kernels.h"
#include <functional>
#include <pthread.h>

preparedImage loadImage(const char *name_file)
{
    preparedImage image;
    std::vector<unsigned char> in_image;
    
    // Load the data
    printf("Ładowanie danych \n");
    unsigned error = lodepng::decode(in_image, image.width, image.height, name_file);
    if (error)
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    // Prepare the data
    printf("Przygotowanie danych \n");
    image.size = in_image.size();
    image.input_image = new unsigned char[(in_image.size() * 3) / 4];
    image.output_image = new unsigned char[(in_image.size() * 3) / 4];
    int where = 0;
    for (int i = 0; i < in_image.size(); ++i)
    {
        if ((i + 1) % 4 != 0)
        {
            image.input_image[where] = in_image.at(i);
            image.output_image[where] = 255;
            where++;
        }
    }
    return image;
}

int main(int argc, char **argv)
{
    // Co drugi wczytuję.
    preparedImage *images = new preparedImage[argc / 2];           // obrazki
    ParametersToFilter *params = new ParametersToFilter[argc / 2]; // parametry do filtrów
    pthread_t *threads = new pthread_t[argc / 2];                  // uchwyty na wątki

    for (int j = 0; j < argc; j += 2)
    {

        // Read the arguments
        printf("Czytanie argumentów: %s %s \n", argv[1 + j], argv[2 + j]);
        const char *input_file = argv[1 + j];
        

        images[j / 2] = loadImage(input_file);
        params[j / 2].images = images;
        params[j / 2].img_id = j / 2;

        // Run the filter on it
        printf("Filter %d uruchom \n", j / 2);
        if (pthread_create(&threads[j/2], NULL, filter, &(params[j/2]))) {
            fprintf(stderr, "Error creating threadn");
            return 1;
        }
    }
    printf("Joining threads \n");
    for (int j = 0; j < argc; j += 2)
    {
        const char *output_file = argv[2 + j];
        if (pthread_join(threads[j / 2], NULL))
        {
            fprintf(stderr, "Error joining threadn");
            return 2;
        }

        printf("Dane na output \n");
        std::vector<unsigned char> out_image;
        for (int i = 0; i < images[j / 2].size; ++i)
        {
            out_image.push_back(images[j / 2].output_image[i]);
            if ((i + 1) % 3 == 0)
            {
                out_image.push_back(255);
            }
        }

        // Output the data
        printf("Output data \n");
        unsigned error = lodepng::encode(output_file, out_image, images[j / 2].width, images[j / 2].height);

        //if there's an error, display it
        if (error)
            std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

        delete[] images[j / 2].input_image;
        delete[] images[j / 2].output_image;
    }
    delete[] images;
    delete[] params;
    delete[] threads;
    return 0;
}
