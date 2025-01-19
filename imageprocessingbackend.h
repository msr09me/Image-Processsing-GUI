#ifndef IMAGE_PROCESSING_BACKEND_H
#define IMAGE_PROCESSING_BACKEND_H

#include <stdint.h>
#include <stdexcept>
#include "ImageIO.h"
#include "IntensityTransformations.h"
#include "ImageFilter.h"

typedef struct {
    uint8_t *data;
    int width;
    int height;
    int bitDepth;
} Image;

Image loadImage(const char *filePath);
void saveImage(const char *filePath, const Image *image);
void applyNegativeB(ImageReadResult *image);
void applyLogTransform(ImageReadResult *image, double c);
void applyGammaTransform(ImageReadResult *image, double c, double gamma);

void boxFilter(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelSize);
void gaussianFilter(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelSize, double sigma);
void medianFilter(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelSize);

void highpassFilter(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelChoice);
void imageSharpening(const ImageReadResult &inputImage, ImageReadResult &outputImage, int kernelChoice);
void umhbf(const ImageReadResult &inputImage, ImageReadResult &outputImage, double k);
#endif // IMAGE_PROCESSING_BACKEND_H
