#include "ImageConverter.h"

std::vector<uint8_t> applyGrayscaleToBinary(const ImageReadResult& inputImage, int threshold) {
    const uint8_t* buffer = inputImage.buffer->data();
    const ImageMetadata& meta = inputImage.meta;

    int rows = meta.height;
    int cols = meta.width;

    // Use the provided output buffer or create a new one
    std::vector<uint8_t> outputBuffer(rows * cols, 0);

    for (int i = 0; i < rows * cols; ++i) {
        outputBuffer[i] = (buffer[i] > threshold) ? 255 : 0; // Set binary values: 255 for white, 0 for black
    }

    return outputBuffer;

}
