#include "IntensityTransformations.h"
#include <cmath> // For log, pow

void applyNegative(uint8_t *buffer, const ImageMetadata &meta) {

    std::cout << "Aplying Negative Transformation...\n";

    int maxVal = (1 << meta.bitDepth) - 1;
    for (int i = 0; i < meta.width * meta.height; i++) {
        buffer[i] = maxVal - buffer[i];
    }

    std::cout << "Completed Negative Transformation...\n";
}

void applyLogTransform(uint8_t *buffer, const ImageMetadata &meta, double c) {
    int maxVal = (1 << meta.bitDepth) - 1;

    std::cout << "\n Applying Log Transformation...\n";

    for (int i = 0; i < meta.width * meta.height; i++) {
        double transformedValue = c * log(1 + buffer[i]);
        if (transformedValue > maxVal) {
            transformedValue = maxVal; // Clamp the value
        }
        buffer[i] = static_cast<uint8_t>(transformedValue);
    }

    std::cout << "Completed Log Transformation...\n";
}

void applyGammaTransform(uint8_t *buffer, const ImageMetadata &meta, double c, double gamma) {
    int maxVal = (1 << meta.bitDepth) - 1;

    std::cout << "\n Applying Gamma Transformation...\n";

    for (int i = 0; i < meta.width * meta.height; i++) {
        double transformedValue = c * pow(buffer[i], gamma);
        if (transformedValue > maxVal) {
            transformedValue = maxVal; // Clamp the value
        }
        buffer[i] = static_cast<uint8_t>(transformedValue);
    }

    std::cout << "Completed Gamma Transformation...\n";
}
