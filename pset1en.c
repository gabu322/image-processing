#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
 */
Image* loadImage(const char* filename) {
    int width, height, channels;
    unsigned char* imgData = stbi_load(filename, &width, &height, &channels, 0);

    if (!imgData) {
        printf("Error loading image: %s\n", filename);
        return NULL;
    }

    Image* img = (Image*)malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->channels = channels;
    img->pixels = imgData;

    printf("Image loaded: %s\n", filename);
    printf("Dimensions: %dx%d, Channels: %d\n", width, height, channels);
    printf("Type: %s\n", (channels == 4) ? "RGBA" : (channels == 3) ? "RGB" : "Black and White");
    return img;
}

// Function to convert RGB or LA image data to Black and White
unsigned char* convertBnW(Image* img) {
    img->channels = 1;
    unsigned char* BnWPixels = (unsigned char*)malloc(img->width * img->height);

    for (int i = 0; i < img->width * img->height; ++i) {
        if (img->channels == 3 || img->channels == 4) {
            // For RGB(A) images
            unsigned char r = img->pixels[i * img->channels];
            unsigned char g = img->pixels[i * img->channels + 1];
            unsigned char b = img->pixels[i * img->channels + 2];
            BnWPixels[i] = (unsigned char)round(0.299 * r + 0.587 * g + 0.114 * b);
        }
        else if (img->channels == 2) {
            // For LA (Luminance + Alpha)
            BnWPixels[i] = img->pixels[i * img->channels];
        }
        else if (img->channels == 1) {
            // For L (Luminance)
            BnWPixels[i] = img->pixels[i];
        }
        else {
            printf("Unsupported image type");
            free(BnWPixels);
            return NULL;
        }
    }
    return BnWPixels;
}

/** @brief Save an image to a file
 *
 * @param filename The name the file will be saved as
 * @param img The image that will be saved
 *
 * @return Returns 0 if the image was saved successfully, and a non-zero value otherwise
 */
void saveImage(const char* filename, Image const* img) {
    bool success = stbi_write_png(filename, img->width, img->height, img->channels, img->pixels, img->width * img->channels);

    if (success) {
        printf("Image saved successfully: %s\n", filename);
    }
    else {
        printf("Error saving image: %s\n", filename);

    }

    return success;
}

int clamp(int value, int min, int max) {
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

Image* applyKernel(const Image* img, const float* kernel, const int kernelSize) {
    int width = img->width;
    int height = img->height;
    int kHalf = kernelSize / 2;

    // Create a 2d array to store the output image
    unsigned char* outputPixels = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    // Create the output image
    Image* output = (Image*)malloc(sizeof(Image));
    output->width = width;
    output->height = height;
    output->pixels = (unsigned char*)malloc(width * height * sizeof(unsigned char));

    // Apply kernel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int sum = 0;

            // Convolution for each kernel position
            for (int ky = -kHalf; ky <= kHalf; ky++) {
                for (int kx = -kHalf; kx <= kHalf; kx++) {
                    int pixelX = clamp(x + kx, 0, width - 1);  // Clamp to valid x range
                    int pixelY = clamp(y + ky, 0, height - 1); // Clamp to valid y range

                    int imageIndex = pixelY * width + pixelX;
                    int kernelIndex = (ky + kHalf) * kernelSize + (kx + kHalf);
                    sum += img->pixels[imageIndex] * kernel[kernelIndex];
                }
            }

            // Store the result in the output image and clamp to [0, 255]
            int outputIndex = y * width + x;
            output->pixels[outputIndex] = (unsigned char)(sum < 0) ? 0 : (sum > 255) ? 255 : sum;
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
    // Create a blur kernel
    int kernelSize = blurLevel * 2 + 1;
    float* kernel = (float*)malloc(kernelSize * kernelSize * sizeof(float));

    for (int i = 0; i < kernelSize * kernelSize; i++) {
        kernel[i] = 1.0f / (kernelSize * kernelSize);
        printf("Kernel value: %f\n", kernel[i]);
    }

    // Apply the kernel to the image
    Image* blurredImage = applyKernel(img, kernel, kernelSize);
    free(kernel);

    return blurredImage;
}


bool compare_images(const Image* img1, const Image* img2) {
    // Ensure images are the same size
    if (img1->width != img2->width || img1->height != img2->height) {
        printf("Images have different dimensions. Cannot compare.\n");
        return -1.0;
    }

    int total_pixels = img1->width * img1->height;

    for (int i = 0; i < total_pixels; ++i) {
        if (img1->pixels[i] != img2->pixels[i]) {
            return false;
        }
    }

    return true;
}

int compareImagesQuantity(const Image* img1, const Image* img2) {
    // Ensure images are the same size
    if (img1->width != img2->width || img1->height != img2->height) {
        printf("Images have different dimensions. Cannot compare.\n");
        return -1.0; // Return -1 for error
    }

    int total_pixels = img1->width * img1->height;
    int differentPixels = 0;

    for (int i = 0; i < total_pixels; ++i) {
        if (img1->pixels[i] != img2->pixels[i]) {
            differentPixels++;
        }
    }

    return differentPixels;
}


// Function to free image resources
void free_image(Image* img) {
    stbi_image_free(img->pixels);
    free(img);
}

int main() {
    Image* baseImage = loadImage("test_images/centered_pixel.png");
    saveImage("output_images/centered_pixel.png", baseImage);
    return 0;
    // declare a string here

}
