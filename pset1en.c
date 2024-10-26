#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "image_functions_en.c"


int main() {
    //Open image twocats.jpg in the test_images folder, transform it to black and white and save it as twocats_bw.png in the output_images folder
    //Then, compare twocats_bw.png with twocats_blur_01.png and print the number of different pixels
    Image* baseImage = loadImage("test_images/twocats.png");
    Image* bwImage = convertBnW(baseImage);

    saveImage("output_images/twocats_bw.png", bwImage);

    Image* blurImage = applyBlur(bwImage, 3);
    saveImage("output_images/twocats_blur_05.png", blurImage);

    Image* test_result_image = loadImage("test_results/twocats_blur_07.png");

    int differentPixels = compareImages(blurImage, test_result_image);
    printf("Images are the same? %d\n", differentPixels);


    free_image(baseImage);
    free_image(bwImage);
    free_image(blurImage);
    free_image(test_result_image);
    return 0;
}
