#include "ImageFilter.h"


// Box Filter ----------------------------------------------------------------------------

std::vector<uint8_t> applyBoxFilter(const ImageReadResult& inputImage, int kernelSize) {
    if (!inputImage.meta.isValid() || !inputImage.buffer.has_value()) {
        throw std::invalid_argument("Invalid image metadata or missing buffer!");
    }

    // Access the buffer
    const uint8_t* buffer = inputImage.buffer->data();
    const ImageMetadata& meta = inputImage.meta;

    int rows = meta.height;
    int cols = meta.width;
    int halfKernel = kernelSize / 2;

    // Create a copy of the buffer for the filtered result
    std::vector<uint8_t> outputBuffer(rows * cols, 0);

    // Apply the box filter
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int sum = 0;
            int count = 0;

            for (int ki = -halfKernel; ki <= halfKernel; ++ki) {
                for (int kj = -halfKernel; kj <= halfKernel; ++kj) {
                    int x = i + ki;
                    int y = j + kj;

                    if (x >= 0 && x < rows && y >= 0 && y < cols) {  
                        /* as we are not using any padding,  
                        x < rows and y < cols ensure that we are not using any pixels or values outside of image boundary
                        so, an image kernel of 3*3 does not mean that we are considering 9 pixels always
                        that's why we need count++ to calculate how many pixels we are considering actually */

                        sum += buffer[x * cols + y];
                        count++;
                    }
                }
            }

            outputBuffer[i * cols + j] = static_cast<uint8_t>(sum / count);
        }
    }

    return outputBuffer;
}

// Gaussian Filter ------------------------------------------------------------------------------------------

std::vector<uint8_t> applyGaussianFilter(const ImageReadResult& inputImage, int kernelSize, double sigma) {
    if (!inputImage.meta.isValid() || !inputImage.buffer.has_value()) {
        throw std::invalid_argument("Invalid image metadata or missing buffer!");
    }

    std::cout << "Gaussian filtering started" <<std::endl;

    const uint8_t* buffer = inputImage.buffer->data();

    std::cout << "buffer data collected" <<std::endl;

    const ImageMetadata& meta = inputImage.meta;

    std::cout << "Meta data collected" <<std::endl;

    int rows = meta.height;
    int cols = meta.width;
    int halfKernel = kernelSize / 2;

    // Create a 2D Gaussian kernel
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize));
    double sum = 0.0;

    for (int i = -halfKernel; i <= halfKernel; ++i) {
        for (int j = -halfKernel; j <= halfKernel; ++j) {
            double value = std::exp(-(i * i + j * j) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
            kernel[i + halfKernel][j + halfKernel] = value;
            sum += value;
        }
    }

    std::cout << "Gaussian kernel created" <<std::endl;

    // Normalize the kernel
    for (int i = 0; i < kernelSize; ++i) {
        for (int j = 0; j < kernelSize; ++j) {
            kernel[i][j] /= sum;
        }
    }

    // Create an output buffer
    std::vector<uint8_t> outputBuffer(rows * cols, 0);

    std::cout << "New buffer created to store the filtered buffer" <<std::endl;

    // Apply the Gaussian filter
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            double weightedSum = 0.0;
            double weightSum = 0.0;

            for (int ki = -halfKernel; ki <= halfKernel; ++ki) {
                for (int kj = -halfKernel; kj <= halfKernel; ++kj) {
                    int x = i + ki;
                    int y = j + kj;

                    if (x >= 0 && x < rows && y >= 0 && y < cols) {
                        double weight = kernel[ki + halfKernel][kj + halfKernel];
                        weightedSum += buffer[x * cols + y] * weight;
                        weightSum += weight;
                    }
                }
            }

            outputBuffer[i * cols + j] = static_cast<uint8_t>(weightedSum / weightSum);
        }
    }

    std::cout << "Applying Gaussian Filter is completed" <<std::endl;

    return outputBuffer;
}


// Median Filter --------------------------------------------------------------------------------------

std::vector<uint8_t> applyMedianFilter(const ImageReadResult& inputImage, int kernelSize) {
    if (!inputImage.meta.isValid() || !inputImage.buffer.has_value()) {
        throw std::invalid_argument("Invalid image metadata or missing buffer!");
    }

    std::cout << "Gaussian filtering started" <<std::endl;

    const uint8_t* buffer = inputImage.buffer->data();

    std::cout << "Buffer data collected" <<std::endl;

    const ImageMetadata& meta = inputImage.meta;

    std::cout << "Meta data collected" <<std::endl;

    int rows = meta.height;
    int cols = meta.width;
    int halfKernel = kernelSize / 2;

    // Create an output buffer initialized to zero
    std::vector<uint8_t> outputBuffer(rows * cols, 0);

    // Temporary vector to store the kernel values for median calculation
    std::vector<uint8_t> window;

    // Apply the median filter
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            window.clear();

            // Collect the neighborhood values into the window
            for (int ki = -halfKernel; ki <= halfKernel; ++ki) {
                for (int kj = -halfKernel; kj <= halfKernel; ++kj) {
                    int x = i + ki;
                    int y = j + kj;

                    // Ensure the indices are within bounds
                    if (x >= 0 && x < rows && y >= 0 && y < cols) {
                        window.push_back(buffer[x * cols + y]);
                    }
                }
            }

            // Find the median value
            std::nth_element(window.begin(), window.begin() + window.size() / 2, window.end());
            uint8_t median = window[window.size() / 2];

            // Assign the median to the output buffer
            outputBuffer[i * cols + j] = median;
        }
    }

    return outputBuffer;
}