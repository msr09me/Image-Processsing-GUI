#include "imageProcessingBackend.h"
#include "ImageIO.h"
#include "IntensityTransformations.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Function to load an image from a file
#include <algorithm> // For std::reverse

Image loadImage(const char *filePath) {
    Image image = {NULL, 0, 0, 0}; // Initialize an empty image
    ImageMetadata meta;

    // Allocate memory for the header and color table
    uint8_t header[HEADER_SIZE];
    uint8_t colorTable[COLOR_TABLE_SIZE];

    // Read the image header and metadata
    if (readImage(filePath, header, colorTable, NULL, &meta, 0) != 0) {
        fprintf(stderr, "Failed to read image metadata.\n");
        return image;
    }

    // Allocate memory for the image data
    size_t bufferSize = meta.width * meta.height * (meta.bitDepth / 8);
    image.data = (uint8_t *)malloc(bufferSize);
    if (!image.data) {
        fprintf(stderr, "Failed to allocate memory for image data.\n");
        return image;
    }

    // Read the image data
    if (readImage(filePath, header, colorTable, image.data, &meta, bufferSize) != 0) {
        fprintf(stderr, "Failed to read image data.\n");
        free(image.data);
        image.data = NULL;
        return image;
    }

    // Flip the image vertically
    int rowSize = meta.width * (meta.bitDepth / 8);
    for (int i = 0; i < meta.height / 2; i++) {
        std::swap_ranges(
            image.data + i * rowSize,
            image.data + (i + 1) * rowSize,
            image.data + (meta.height - 1 - i) * rowSize
            );
    }

    // Set image properties
    image.width = meta.width;
    image.height = meta.height;
    image.bitDepth = meta.bitDepth;

    printf("Loaded image bit depth: %d\n", image.bitDepth);
    return image;
}


// Function to save an image to a file
void saveImage(const char *filePath, const Image *image) {
    uint8_t header[HEADER_SIZE] = {0};
    uint8_t colorTable[COLOR_TABLE_SIZE] = {0};
    ImageMetadata meta = {image->width, image->height, image->bitDepth};

    if (writeImage(filePath, header, colorTable, image->data, &meta, image->width * image->height * (meta.bitDepth / 8)) != 0) {
        fprintf(stderr, "Failed to save image.\n");
    }
}

// Function to apply a negative transformation
void applyNegativeB(Image *image) {
    applyNegative(image->data, image->width, image->height, image->bitDepth);
}


// Function to apply a logarithmic transformation
void applyLogTransform(Image *image, double c) {
    applyLogTransform(image->data, image->width, image->height, image->bitDepth, c);
}

// Function to apply a gamma transformation
void applyGammaTransform(Image *image, double c, double gamma) {
    applyGammaTransform(image->data, image->width, image->height, image->bitDepth, c, gamma);
}

// Function to free allocated image memory
void freeImage(Image *image) {
    if (image->data) {
        free(image->data);
        image->data = NULL;
    }
}
