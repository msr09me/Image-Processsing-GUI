#ifndef IMAGE_PROCESSING_BACKEND_H
#define IMAGE_PROCESSING_BACKEND_H

#include <stdint.h>

typedef struct {
    uint8_t *data;
    int width;
    int height;
    int bitDepth;
} Image;

Image loadImage(const char *filePath);
void saveImage(const char *filePath, const Image *image);
void applyNegativeB(Image *image);
void applyLogTransform(Image *image, double c);
void applyGammaTransform(Image *image, double c, double gamma);
void freeImage(Image *image);

#endif // IMAGE_PROCESSING_BACKEND_H
