#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "image_functions_en.c"


int main() {
    Image* img = loadImage("test_images/twocats.png");

    float kernel[] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0.5, 0.5, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0
    };

    img = convertBnW(img);
    img = applySharpen(img, 4);

    Image* compareImage = loadImage("test_results/twocats_sharp_09.png");

    if (compareImage == NULL) {
        printf("Error loading compare image\n");
        freeImage(img);
        return 1;
    }

    bool result = compareImages(img, compareImage);

    if (result) {
        printf("Images are the same\n");
    }
    else {
        printf("Images are different\n");
    }

    saveImage("output_images/blob.png", img);

    freeImage(img);
    return 0;
}
