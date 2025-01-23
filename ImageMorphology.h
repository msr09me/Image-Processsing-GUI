#ifndef IMAGE_MORPHOLOGY_H
#define IMAGE_MORPHOLOGY_H

#include <vector>
#include <cstdint>
#include "ImageIO.h"

// Function prototypes for morphological operations
std::vector<uint8_t> applyErosion(const ImageReadResult& inputImage, int kernelColumns, int kernelRows);
std::vector<uint8_t> applyDilation(const ImageReadResult& inputImage, int kernelColumns, int kernelRows);
std::vector<uint8_t> applyOpening(const ImageReadResult& inputImage, int kernelColumns, int kernelRows);
std::vector<uint8_t> applyClosing(const ImageReadResult& inputImage, int kernelColumns, int kernelRows);

#endif // IMAGE_MORPHOLOGY_H
