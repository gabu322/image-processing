// #include "image_functions_en.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // For loading images
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" // For saving images

// Structure to hold image information
typedef struct {
    int width;
    int height;
    int channels;
    unsigned char* pixels;
} Image;


/** @brief Load an image from a file
 *
 * @param filename The name of the file to load
 *
 * @return Returns a pointer to the loaded image, or NULL if the image could not be loaded
 */
Image* loadImage(const char* filename) {
    int width, height, channels;

    unsigned char* imgData = stbi_load(filename, &width, &height, &channels, 0);
    if (!imgData) {
        printf("Error loading image: %s\n", filename);
        return NULL;
    }

    Image* img = (Image*)malloc(sizeof(Image));
    if (!img) {
        printf("Error allocating memory for image structure.\n");
        stbi_image_free(imgData);
        return NULL;
    }

    img->width = width;
    img->height = height;
    img->channels = channels;
    img->pixels = imgData;

    printf("Image loaded: %s, dimensions: %d x %d, channels: %d\n", filename, width, height, channels);
    return img;
}

/** @brief Save an image to a file
 *
 * @param filename The name the file will be saved as
 * @param img The image that will be saved
 *
 */
void saveImage(const char* filename, const Image* img) {
    bool imageSaved = stbi_write_png(filename, img->width, img->height, img->channels, img->pixels, img->width * img->channels);

    if (imageSaved) {
        printf("Image saved successfully: %s\n", filename);
    }
    else {
        printf("Error saving image: %s\n", filename);
    }
}

/** @brief Free the memory allocated for an image
 *
 * @param img The image that will be freed
 */
void freeImage(Image* img) {
    stbi_image_free(img->pixels);
    free(img);
}

/** @brief Clamp a value to a range
 *
 * @param value The value that will be clamped
 * @param min The minimum value
 * @param max The maximum value
 *
 * @return The clamped value
 */
int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

/** @brief Invert the colors of an image
 *
 * @param img The image that will be inverted
 *
 * @return The inverted image
 */
Image* invertPixels(const Image* img) {
    Image* invertedImage = (Image*)malloc(sizeof(Image));
    if (!invertedImage) {
        printf("Error allocating memory for inverted image\n");
        return NULL;
    }

    invertedImage->width = img->width;
    invertedImage->height = img->height;
    invertedImage->channels = img->channels;
    invertedImage->pixels = (unsigned char*)malloc(img->width * img->height * img->channels * sizeof(unsigned char));
    if (!invertedImage->pixels) {
        free(invertedImage);
        printf("Error allocating memory for inverted image pixels\n");
        return NULL;
    }

    // Invert the pixel values
    for (int i = 0; i < img->width * img->height * img->channels; ++i) {
        invertedImage->pixels[i] = 255 - img->pixels[i];
    }

    return invertedImage;
}

/** @brief Convert an image to black and white
 *
 * @param img The image that will be converted
 *
 * @return The black and white image
 */
Image* convertBnW(const Image* img) {
    Image* convertedImage = (Image*)malloc(sizeof(Image));
    if (!convertedImage) {
        printf("Error allocating memory for converted image\n");
        return NULL;
    }

    unsigned char* BnWPixels = (unsigned char*)malloc(img->width * img->height * sizeof(unsigned char));
    if (!BnWPixels) {
        printf("Error allocating memory for BnW pixels\n");
        free(convertedImage);
        return NULL;
    }

    // Convert the pixel to black and white based on the number of channels
    for (int i = 0; i < img->width * img->height; ++i) {
        if (img->channels == 3 || img->channels == 4) {
            unsigned char r = img->pixels[i * img->channels];
            unsigned char g = img->pixels[i * img->channels + 1];
            unsigned char b = img->pixels[i * img->channels + 2];
            BnWPixels[i] = (unsigned char)round(0.299 * r + 0.587 * g + 0.114 * b);
        }
        else if (img->channels == 2) {
            BnWPixels[i] = img->pixels[i * img->channels];
        }
        else if (img->channels == 1) {
            BnWPixels[i] = img->pixels[i];
        }
        else {
            printf("Unsupported image type");
            free(BnWPixels);
            free(convertedImage);
            return NULL;
        }
    }

    convertedImage->width = img->width;
    convertedImage->height = img->height;
    convertedImage->channels = 1;
    convertedImage->pixels = BnWPixels;

    return convertedImage;
}

/** @brief Apply a kernel to an image
 *
 * @param img The image that will be applied the kernel
 * @param kernel The kernel that will be applied to the image
 * @param kernelSize The size of one side of the kernel
 *
 * @return The image after the kernel has been applied
 */
Image* applyKernel(const Image* img, const float* kernel, const int kernelSize) {
    Image* output = (Image*)malloc(sizeof(Image));
    if (!output) {
        printf("Error allocating memory for output image\n");
        return NULL;
    }

    output->width = img->width;
    output->height = img->height;
    output->channels = img->channels;
    output->pixels = (unsigned char*)malloc(img->width * img->height * img->channels * sizeof(unsigned char));
    if (!output->pixels) {
        free(output);
        printf("Error allocating memory for output image\n");
        return NULL;
    }

    // Traverse each pixel in the image
    for (int imgY = 0; imgY < img->height; imgY++) {
        for (int imgX = 0; imgX < img->width; imgX++) {
            // Process each channel (e.g., R, G, B for RGB)
            for (int channelIndex = 0; channelIndex < img->channels; channelIndex++) {
                float pixelValue = 0.0f;

                // Traverse each position in the kernel
                for (int kernelY = -kernelSize / 2; kernelY <= kernelSize / 2; kernelY++) {
                    for (int kernelX = -kernelSize / 2; kernelX <= kernelSize / 2; kernelX++) {

                        // Calculate the position of the neighboring pixel, clamped to image boundaries
                        int pixelX = clamp(imgX + kernelX, 0, img->width - 1);
                        int pixelY = clamp(imgY + kernelY, 0, img->height - 1);

                        // Calculate the position (index) in the image and kernel
                        int pixelIndex = (pixelY * img->width + pixelX) * img->channels + channelIndex;
                        int kernelIndex = (kernelY + kernelSize / 2) * kernelSize + (kernelX + kernelSize / 2);

                        pixelValue += img->pixels[pixelIndex] * kernel[kernelIndex];
                    }
                }

                // Clamp the pixel value to the 0-255 range
                pixelValue = clamp((int)pixelValue, 0, 255);

                // Set the output pixel value
                int outputIndex = (imgY * img->width + imgX) * img->channels + channelIndex;
                output->pixels[outputIndex] = (unsigned char)pixelValue;
            }
        }
    }

    return output;
}


/** @brief Apply a blur effect to an image, based on a value of blurLevel that will transform in a kernel of size (blurLevel * 2 + 1)
 *
 * @param img The image that will be applied the blur
 * @param blurLevel: The amount of blur applied (starting at 1)
 *
 * @return The blurred image
 */
Image* applyBlur(const Image* img, int blurLevel) {
    if (blurLevel < 1) {
        printf("Blur level must be at least 1\n");
        return NULL;
    }

    int kernelSize = blurLevel * 2 + 1;
    float* kernel = (float*)malloc(kernelSize * kernelSize * sizeof(float));
    if (!kernel) {
        printf("Error allocating memory for kernel\n");
        return NULL;
    }

    // Create a kernel with equal weights
    float weight = 1.0f / (kernelSize * kernelSize);
    for (int i = 0; i < kernelSize; i++) {
        for (int j = 0; j < kernelSize; j++) {
            kernel[i * kernelSize + j] = weight;
        }
    }

    Image* blurredImage = applyKernel(img, kernel, kernelSize);

    free(kernel);

    return blurredImage;
}

/** @brief Apply a sharpen effect to an image
 *
 * @param img The image that will be applied the sharpen effect
 *
 * @return The sharpened image
 */
Image* applySharpen(const Image* img, int sharpenLevel) {
    if (sharpenLevel < 1) {
        printf("Sharpen level must be at least 1\n");
        return NULL;
    }

    Image* sharpenedImage = (Image*)malloc(sizeof(Image));
    if (!sharpenedImage) {
        printf("Error allocating memory for sharpened image\n");
        return NULL;
    }

    sharpenedImage->width = img->width;
    sharpenedImage->height = img->height;
    sharpenedImage->channels = img->channels;
    sharpenedImage->pixels = (unsigned char*)malloc(img->width * img->height * img->channels * sizeof(unsigned char));
    if (!sharpenedImage->pixels) {
        free(sharpenedImage);
        printf("Error allocating memory for sharpened image pixels\n");
        return NULL;
    }

    Image* blurredImage = applyBlur(img, sharpenLevel);

    // Traverse each pixel in the image
    for (int imgY = 0; imgY < img->height; imgY++) {
        for (int imgX = 0; imgX < img->width; imgX++) {
            // Process each channel (e.g., R, G, B for RGB)
            for (int channelIndex = 0; channelIndex < img->channels; channelIndex++) {
                int imageIndex = (imgY * img->width + imgX) * img->channels + channelIndex;

                // Calculate the sharpened pixel value and clamp it to the 0-255 range
                int sharpenedValue = clamp((img->pixels[imageIndex] * 2) - blurredImage->pixels[imageIndex], 0, 255);

                // Set the output pixel value
                sharpenedImage->pixels[imageIndex] = (unsigned char)sharpenedValue;
            }
        }
    }

    return sharpenedImage;
}

/** @brief Apply an edge detection effect to an image
 *
 * @param img The image that will be applied the edge detection effect
 *
 * @return The image after the edge detection has been applied
 */
Image* applyEdgeDetection(const Image* img) {
    Image* outputImage = (Image*)malloc(sizeof(Image));
    if (!outputImage) {
        printf("Error allocating memory for output image\n");
        return NULL;
    }

    outputImage->width = img->width;
    outputImage->height = img->height;
    outputImage->channels = img->channels;
    outputImage->pixels = (unsigned char*)malloc(img->width * img->height * img->channels * sizeof(unsigned char));
    if (!outputImage->pixels) {
        free(outputImage);
        printf("Error allocating memory for output image\n");
        return NULL;
    }

    float KX[] = {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    };

    float KY[] = {
        -1, -2, -1,
        0, 0, 0,
        1, 2, 1
    };

    // Traverse each pixel in the image
    for (int imgY = 0; imgY < img->height; imgY++) {
        for (int imgX = 0; imgX < img->width; imgX++) {
            // Process each channel (e.g., R, G, B for RGB)
            for (int channelIndex = 0; channelIndex < img->channels; channelIndex++) {
                float pixelValueX = 0.0f;
                float pixelValueY = 0.0f;

                // Traverse each position in the kernel
                for (int kernelY = -1; kernelY <= 1; kernelY++) {
                    for (int kernelX = -1; kernelX <= 1; kernelX++) {

                        // Calculate the position of the neighboring pixel, clamped to image boundaries
                        int pixelX = clamp(imgX + kernelX, 0, img->width - 1);
                        int pixelY = clamp(imgY + kernelY, 0, img->height - 1);

                        // Calculate the position (index) in the image and kernel
                        int pixelIndex = (pixelY * img->width + pixelX) * img->channels + channelIndex;
                        int kernelIndex = (kernelY + 1) * 3 + (kernelX + 1);

                        pixelValueX += img->pixels[pixelIndex] * KX[kernelIndex];
                        pixelValueY += img->pixels[pixelIndex] * KY[kernelIndex];
                    }
                }

                // Do the calculation of the pixel value and clamp it to the 0-255 range
                int pixelCalculatedValue = clamp(sqrt(pixelValueX * pixelValueX + pixelValueY * pixelValueY), 0, 255);

                // Set the output pixel value
                int outputIndex = (imgY * img->width + imgX) * img->channels + channelIndex;
                outputImage->pixels[outputIndex] = (unsigned char)pixelCalculatedValue;
            }
        }
    }

    return outputImage;
}

/** @brief Compare two images to determine if they are the same (within a 1 degree of tolerance)
 *
 * @param img1 The first image to compare
 * @param img2 The second image to compare
 *
 * @return Returns true if the images are the same, and false otherwise
 */
bool compareImages(const Image* img1, const Image* img2) {
    // Ensure images are the same size
    if (img1->width != img2->width || img1->height != img2->height) {
        printf("Images have different dimensions. Cannot compare.\n");
        return -1.0;
    }

    // If it finds a pixel that is different (with a tolerance), return false
    for (int i = 0; i < img1->width * img1->height; ++i) {
        if (abs(img1->pixels[i] - img2->pixels[i]) > 1) {
            return false;
        }
    }

    return true;
}
