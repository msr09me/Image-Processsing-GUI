#include "IntensityTransformations.h"
#include <cmath> // For log, pow
#include <stdio.h>

void applyNegative(uint8_t *buffer, int width, int height, int bitDepth) {
    int maxVal = (1 << bitDepth) - 1;
    for (int i = 0; i < width * height; i++) {

        uint8_t originalPixel = buffer[i];

        buffer[i] = maxVal - buffer[i];
        /*

        if (i > 10 && i % (width * height / 10) == 0) {

            printf("Pixel[%d]: Original = %d, Transformed = %d\n", i, originalPixel, buffer[i]);
        }
*/

    }
}

void applyLogTransform(uint8_t *buffer, int width, int height, int bitDepth, double c) {
    int maxVal = (1 << bitDepth) - 1;
    for (int i = 0; i < width * height; i++) {
        double transformedValue = c * log(1 + buffer[i]);
        if (transformedValue > maxVal) {
            transformedValue = maxVal; // Clamp the value
        }
        buffer[i] = static_cast<uint8_t>(transformedValue);
    }
}

void applyGammaTransform(uint8_t *buffer, int width, int height, int bitDepth, double c, double gamma) {
    int maxVal = (1 << bitDepth) - 1;
    for (int i = 0; i < width * height; i++) {
        double normalized = buffer[i] / (double)maxVal; // Normalize to [0, 1]
        double transformed = c * pow(normalized, gamma);
        buffer[i] = static_cast<uint8_t>(std::min(transformed * maxVal, (double)maxVal));
    }
}
