#include "imageProcessingBackend.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Function to load an image from a file
#include <algorithm> // For std::reverse

// Function to save an image to a file
void saveImage(const char *filePath, const ImageReadResult *image) {

}

// Function to apply a negative transformation
void applyNegativeB(ImageReadResult *image) {
    applyNegative(image->buffer->data(), image->meta);
}



// Function to apply a logarithmic transformation
void applyLogTransform(ImageReadResult *image, double c) {
    applyLogTransform(image->buffer->data(), image->meta, c);
}


// Function to apply a gamma transformation
void applyGammaTransform(ImageReadResult *image, double c, double gamma) {
    applyGammaTransform(image->buffer->data(), image->meta, c, gamma);
}

// Function to apply a box filter

void boxFilter(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelSize) {
    if (!inputImage.buffer.has_value() || !inputImage.meta.isValid()) {
        throw std::invalid_argument("Invalid input image!");
    }

    try {
        auto filteredBuffer = applyBoxFilter(inputImage, kernelSize);
        outputImage = inputImage; // Copy metadata and other details
        outputImage.buffer = std::make_optional(filteredBuffer);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Box Filter failed: ") + e.what());
    }
}

void gaussianFilter(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelSize, double sigma) {
    if (!inputImage.buffer.has_value() || !inputImage.meta.isValid()) {
        throw std::invalid_argument("Invalid input image!");
    }

    try {
        auto filteredBuffer = applyGaussianFilter(inputImage, kernelSize, sigma);
        outputImage = inputImage; // Copy metadata and other details
        outputImage.buffer = std::make_optional(filteredBuffer);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Gaussian Filter failed: ") + e.what());
    }
}


void medianFilter(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelSize) {
    if (!inputImage.buffer.has_value() || !inputImage.meta.isValid()) {
        throw std::invalid_argument("Invalid input image!");
    }

    try {
        auto filteredBuffer = applyMedianFilter(inputImage, kernelSize);
        outputImage = inputImage; // Copy metadata and other details
        outputImage.buffer = std::make_optional(filteredBuffer);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Median Filter failed: ") + e.what());
    }
}


void highpassFilter(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelChoice){
    if (!inputImage.buffer.has_value() || !inputImage.meta.isValid()) {
        throw std::invalid_argument("Invalid input image!");
    }

    try {
        auto filteredBuffer = applyHighPassFilter(inputImage, kernelChoice);
        outputImage = inputImage; // copy the metadata
        outputImage.buffer = std::make_optional(filteredBuffer);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Highpass Filter failed: ") + e.what());
    }
}

void imageSharpening(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelChoice){
    if (!inputImage.buffer.has_value() || !inputImage.meta.isValid()) {
        throw std::invalid_argument("Invalid input image!");
    }

    try {
        auto filteredBuffer = applyImageSharpening(inputImage, kernelChoice);
        outputImage = inputImage; // copy the metadata
        outputImage.buffer = std::make_optional(filteredBuffer);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Image sharpening failed: ") + e.what());
    }
}

void umhbf(const ImageReadResult &inputImage, ImageReadResult &outputImage, double k){
    if (!inputImage.buffer.has_value() || !inputImage.meta.isValid()) {
        throw std::invalid_argument("Invalid input image!");
    }

    try {
        auto filteredBuffer = applyUMHBF(inputImage, k);
        outputImage = inputImage; // copy the metadata
        outputImage.buffer = std::make_optional(filteredBuffer);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Image sharpening failed: ") + e.what());
    }
}
