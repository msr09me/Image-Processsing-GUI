#ifndef IMAGE_CONVERTER_H
#define IMAGE_CONVERTER_H

#include <vector>       // To use vector
#include <cstdint>      // To use uint8_t

#include "ImageIO.h"    // To use ImageReadResult struct

// Grayscale to binary
std::vector<uint8_t> applyGrayscaleToBinary(const ImageReadResult& inputImage, int threshold);


#endif