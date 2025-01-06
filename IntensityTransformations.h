#ifndef IMAGE_TRANSFORMS_H
#define IMAGE_TRANSFORMS_H

#include <stdint.h> // For uint8_t

void applyNegative(uint8_t *buffer, int width, int height, int bitDepth);
void applyLogTransform(uint8_t *buffer, int width, int height, int bitDepth, double c);
void applyGammaTransform(uint8_t *buffer, int width, int height, int bitDepth, double c, double gamma);

#endif // IMAGE_TRANSFORMS_H
