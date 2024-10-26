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
    bool imageSaved = stbi_write_png(filename, img->width, img->height, 1, img->pixels, img->width);

    if (imageSaved) {
        printf("Image saved successfully: %s\n", filename);
    }
    else {
        printf("Error saving image: %s\n", filename);
    }
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
    output->pixels = (unsigned char*)malloc(img->width * img->height * sizeof(unsigned char));
    if (!output->pixels) {
        free(output);
        printf("Error allocating memory for output image\n");
        return NULL;
    }


    // Pass through each pixel in the image
    for (int imgY = 0; imgY < img->height; imgY++) {
        for (int imgX = 0; imgX < img->width; imgX++) {
            float pixelCalculatedValue = 0.0f;

            // Pass through each pixel in the kernel
            for (int kernelY = -kernelSize / 2; kernelY <= kernelSize / 2; kernelY++) {
                for (int kernelX = -kernelSize / 2; kernelX <= kernelSize / 2; kernelX++) {

                    // Calculate the position of the kernel pixel and clamp it to the image boundaries
                    int kernelPosX = imgX + kernelX;
                    if (kernelPosX < 0) kernelPosX = 0;
                    if (kernelPosX >= img->width) kernelPosX = img->width - 1;

                    int kernelPosY = imgY + kernelY;
                    if (kernelPosY < 0) kernelPosY = 0;
                    if (kernelPosY >= img->height) kernelPosY = img->height - 1;

                    // Calculate the position (index) of the pixel in the image and the kernel
                    int imagIndex = kernelPosX + img->width * kernelPosY;
                    int kernelIndex = (kernelY + kernelSize / 2) * kernelSize + (kernelX + kernelSize / 2);

                    pixelCalculatedValue += img->pixels[imagIndex] * kernel[kernelIndex];
                }
            }

            // Clamp the pixel value to the 0-255 range
            if (pixelCalculatedValue < 0) pixelCalculatedValue = 0;
            if (pixelCalculatedValue > 255) pixelCalculatedValue = 255;

            output->pixels[imgX + img->width * imgY] = (unsigned char)pixelCalculatedValue;
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

/** @brief Free the memory allocated for an image
 *
 * @param img The image that will be freed
 */
void free_image(Image* img) {
    stbi_image_free(img->pixels);
    free(img);
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
