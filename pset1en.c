#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "image_functions_en.c"


int main() {
    // Load the base image and convert it to black and white
    Image* baseImage = loadImage("test_images/chess.png");
    baseImage = convertBnW(baseImage);

    // Load the test image
    Image* comparasionImage = loadImage("test_results/chess_blur_01.png");

    // Compare the images: black and white comparasion
    bool result = compareImages(baseImage, comparasionImage);
    printf("Black and white comparasion: images are %s\n", result ? "the same" : "different");
    freeImage(comparasionImage);

    // Compare the images: blur 3x3 comparasion
    Image* testImage = applyBlur(baseImage, 1);
    comparasionImage = loadImage("test_results/chess_blur_03.png");
    result = compareImages(testImage, comparasionImage);
    printf("Blur comparasion 3x3: images are %s\n", result ? "the same" : "different");
    freeImage(comparasionImage);

    // Compare the images: blur 7x7 comparasion
    testImage = applyBlur(baseImage, 3);
    comparasionImage = loadImage("test_results/chess_blur_07.png");
    result = compareImages(testImage, comparasionImage);
    printf("Blur comparasion 7x7: images are %s\n", result ? "the same" : "different");
    freeImage(comparasionImage);

    // Compare the images: blur 7x7 comparasion
    testImage = applyBlur(baseImage, 3);
    comparasionImage = loadImage("test_results/chess_blur_07.png");
    result = compareImages(testImage, comparasionImage);
    printf("Blur comparasion 9x9: images are %s\n", result ? "the same" : "different");
    freeImage(comparasionImage);

    // Compare the images: invert colors comparasion
    testImage = invertPixels(baseImage);
    comparasionImage = loadImage("test_results/chess_invert.png");
    result = compareImages(testImage, comparasionImage);
    printf("Invert colors comparasion: images are %s\n", result ? "the same" : "different");
    freeImage(comparasionImage);

    // Compare the images: edges detection comparasion
    testImage = applyEdgeDetection(baseImage);
    comparasionImage = loadImage("test_results/chess_edges.png");
    result = compareImages(testImage, comparasionImage);
    printf("Edges detection comparasion: images are %s\n", result ? "the same" : "different");
    freeImage(comparasionImage);

    // Compare the images: sharpen 3x3 comparasion
    testImage = applySharpen(baseImage, 1);
    comparasionImage = loadImage("test_results/chess_sharp_03.png");
    result = compareImages(testImage, comparasionImage);
    printf("Sharpen comparasion 3x3: images are %s\n", result ? "the same" : "different");
    freeImage(comparasionImage);

    // Compare the images: sharpen 9x9 comparasion
    testImage = applySharpen(baseImage, 4);
    comparasionImage = loadImage("test_results/chess_sharp_09.png");
    result = compareImages(testImage, comparasionImage);
    printf("Sharpen comparasion 9x9: images are %s\n", result ? "the same" : "different");
    freeImage(comparasionImage);


    freeImage(baseImage);
    return 0;
}
