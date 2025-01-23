#include "ImageMorphology.h"

// Erosion
std::vector<uint8_t> applyErosion(const ImageReadResult& inputImage, int kernelColumns, int kernelRows) {
    const uint8_t* buffer = inputImage.buffer->data();
    const ImageMetadata& meta = inputImage.meta;

    int rows = meta.height;
    int cols = meta.width;
    int halfKernelColumns = kernelColumns / 2;
    int halfKernelRows = kernelRows / 2;

    std::vector<uint8_t> outputBuffer(rows * cols, 255);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            uint8_t minValue = 255;

            for (int ki = -halfKernelRows; ki <= halfKernelRows; ++ki) {
                for (int kj = -halfKernelColumns; kj <= halfKernelColumns; ++kj) {
                    int x = i + ki;
                    int y = j + kj;

                    if (x >= 0 && x < rows && y >= 0 && y < cols) {
                        minValue = std::min(minValue, buffer[x * cols + y]);
                    }
                }
            }

            outputBuffer[i * cols + j] = minValue;
        }
    }

    return outputBuffer;
}

// Dilation
std::vector<uint8_t> applyDilation(const ImageReadResult& inputImage, int kernelColumns, int kernelRows) {
    const uint8_t* buffer = inputImage.buffer->data();
    const ImageMetadata& meta = inputImage.meta;

    int rows = meta.height;
    int cols = meta.width;
    int halfKernelColumns = kernelColumns / 2;
    int halfKernelRows = kernelRows / 2;

    std::vector<uint8_t> outputBuffer(rows * cols, 255);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            uint8_t maxValue = 0;

            for (int ki = -halfKernelRows; ki <= halfKernelRows; ++ki) {
                for (int kj = -halfKernelColumns; kj <= halfKernelColumns; ++kj) {
                    int x = i + ki;
                    int y = j + kj;

                    if (x >= 0 && x < rows && y >= 0 && y < cols) {
                        maxValue = std::max(maxValue, buffer[x * cols + y]);
                    }
                }
            }

            outputBuffer[i * cols + j] = maxValue;
        }
    }

    return outputBuffer;
}

// Opening: Erosion followed by Dilation
std::vector<uint8_t> applyOpening(const ImageReadResult& inputImage, int kernelColumns, int kernelRows) {
    ImageReadResult tempImage = inputImage;
    tempImage.buffer = applyErosion(inputImage, kernelColumns, kernelRows);

    return applyDilation(tempImage, kernelColumns, kernelRows);
}

// Closing: Dilation followed by Erosion
std::vector<uint8_t> applyClosing(const ImageReadResult& inputImage, int kernelColumns, int kernelRows) {
    ImageReadResult tempImage = inputImage;
    tempImage.buffer = applyDilation(inputImage, kernelColumns, kernelRows);

    return applyErosion(tempImage, kernelColumns, kernelRows);
}
