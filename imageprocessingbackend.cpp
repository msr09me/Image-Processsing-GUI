#include "imageProcessingBackend.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdexcept>
#include "IntensityTransformations.h"
#include "ImageFilter.h"
#include "ImageConverter.h"
#include "ImageMorphology.h"
#include "ImageUtils.h"
#include "ImageEdgeDetection.h"

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
        throw std::runtime_error(std::string("Unsharp masking and highboost filtering failed: ") + e.what());
    }
}

void grayscaleToBinary(const ImageReadResult &inputImage, ImageReadResult &outputImage, int threshold){
    if (!inputImage.buffer.has_value() || !inputImage.meta.isValid()) {
        throw std::invalid_argument("Invalid input image!");
    }
    try {
        auto convertedBuffer = applyGrayscaleToBinary(inputImage, threshold);
        outputImage = inputImage; // copy the metadata
        outputImage.buffer = std::make_optional(convertedBuffer);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Image conversion failed: ") + e.what());
    }
}

// Morphology

void erosion(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelCols, int kernelRows){
    if (!inputImage.buffer.has_value() || !inputImage.meta.isValid()) {
        throw std::invalid_argument("Invalid input image!");
    }
    try {
        auto convertedBuffer = applyErosion(inputImage, kernelCols, kernelRows);
        outputImage = inputImage;
        outputImage.buffer = std::make_optional(convertedBuffer);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Image erosion failed: ") + e.what());
    }
}

void dilation(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelCols, int kernelRows){
    if (!inputImage.buffer.has_value() || !inputImage.meta.isValid()) {
        throw std::invalid_argument("Invalid input image!");
    }
    try {
        auto convertedBuffer = applyDilation(inputImage, kernelCols, kernelRows);
        outputImage = inputImage;
        outputImage.buffer = std::make_optional(convertedBuffer);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Image dilation failed: ") + e.what());
    }
}

void opening(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelCols, int kernelRows){
    if (!inputImage.buffer.has_value() || !inputImage.meta.isValid()) {
        throw std::invalid_argument("Invalid input image!");
    }
    try {
        auto convertedBuffer = applyOpening(inputImage, kernelCols, kernelRows);
        outputImage = inputImage;
        outputImage.buffer = std::make_optional(convertedBuffer);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Image opening failed: ") + e.what());
    }
}

void closing(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelCols, int kernelRows){
    if (!inputImage.buffer.has_value() || !inputImage.meta.isValid()) {
        throw std::invalid_argument("Invalid input image!");
    }
    try {
        auto convertedBuffer = applyClosing(inputImage, kernelCols, kernelRows);
        outputImage = inputImage;
        outputImage.buffer = std::make_optional(convertedBuffer);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Image closing failed: ") + e.what());
    }
}

void boundaryExtraction(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelCols, int kernelRows){
    if (!inputImage.buffer.has_value() || !inputImage.meta.isValid()) {
        throw std::invalid_argument("Invalid input image!");
    }
    try {
        auto convertedBuffer = applyBoundaryExtraction(inputImage, kernelCols, kernelRows);
        outputImage = inputImage;
        outputImage.buffer = std::make_optional(convertedBuffer);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Boundary extraction failed: ") + e.what());
    }
}

// Edge Detection

void gradientEdgeDetection(const ImageReadResult &inputImage, ImageReadResult &outputImage,
                           KernelChoice kernelChoice, PaddingChoice paddingChoice,
                           bool applyThreshold, double thresholdValue){

    if (!inputImage.buffer.has_value() || !inputImage.meta.isValid()) {
        throw std::invalid_argument("Invalid input image!");
    }
    try {
        auto convertedBuffer = applyGradientEdgeDetection(inputImage, kernelChoice, applyThreshold, thresholdValue, paddingChoice);
        outputImage = inputImage;
        outputImage.buffer = std::make_optional(convertedBuffer);
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Boundary extraction failed: ") + e.what());
    }

}




