#include "ImageEdgeDetection.h"
#include "ImageFilter.h"
#include <algorithm>       // for std::clamp (C++17) or remove if you have a custom clamp
#include <cmath>           // for std::sqrt
#include <cstring>         // for std::memcpy, if needed

std::vector<uint8_t> applyGradientEdgeDetection(
    const ImageReadResult& inputImage,
    KernelChoice kernelChoice,
    bool applyThreshold,
    double thresholdValue,
    PaddingChoice paddingChoice
) {
    // 1. Validate input
    if (!inputImage.meta.isValid() || !inputImage.buffer.has_value()) {
        throw std::invalid_argument("Invalid image or missing buffer!");
    }

    const uint8_t* buffer = inputImage.buffer->data();
    const ImageMetadata& meta = inputImage.meta;

    int rows = meta.height;
    int cols = meta.width;

    // 2. Determine kernel type & size
    //    - Sobel & Prewitt are 3x3 => padSize = 1
    //    - Roberts is 2x2 => padSize = 1 as well
    //      (We could do padSize=1 for all these kernels if we want.)

    bool isRoberts = false;
    int padSize = 1; // for 3x3 or 2x2

    // Prepare Gx, Gy arrays
    int gx3[3][3] = {0};
    int gy3[3][3] = {0};

    int gx2[2][2] = {0};
    int gy2[2][2] = {0};

    switch (kernelChoice) {
        case KernelChoice::SOBEL: {
            int tempGx[3][3] = {
                {-1, 0, +1},
                {-2, 0, +2},
                {-1, 0, +1}
            };
            int tempGy[3][3] = {
                {-1, -2, -1},
                { 0,  0,  0},
                {+1, +2, +1}
            };
            std::memcpy(gx3, tempGx, sizeof(gx3));
            std::memcpy(gy3, tempGy, sizeof(gy3));
            break;
        }
        case KernelChoice::PREWITT: {
            int tempGx[3][3] = {
                {-1, 0, +1},
                {-1, 0, +1},
                {-1, 0, +1}
            };
            int tempGy[3][3] = {
                {-1, -1, -1},
                { 0,  0,  0},
                {+1, +1, +1}
            };
            std::memcpy(gx3, tempGx, sizeof(gx3));
            std::memcpy(gy3, tempGy, sizeof(gy3));
            break;
        }
        case KernelChoice::ROBERTS: {
            isRoberts = true;
            int tempGx[2][2] = {
                {+1,  0},
                { 0, -1}
            };
            int tempGy[2][2] = {
                { 0, +1},
                {-1,  0}
            };
            std::memcpy(gx2, tempGx, sizeof(gx2));
            std::memcpy(gy2, tempGy, sizeof(gy2));
            break;
        }
        default:
            throw std::invalid_argument("Unknown kernel choice!");
    }

    // 3. Physically pad the image (if requested)
    //    We'll create a new buffer "paddedBuffer" with dimension (rows + 2*padSize) x (cols + 2*padSize),
    //    or if paddingChoice == NONE, we just use the original buffer (but then we must skip edges in the convolution).
    
    std::vector<uint8_t> paddedBuffer;
    int paddedRows = rows + 2 * padSize;
    int paddedCols = cols + 2 * padSize;

    // If user chooses no padding, we won't physically expand. We'll handle edges by skipping them.
    if (paddingChoice == PaddingChoice::NONE) {
        // Just copy the original data
        paddedBuffer = *inputImage.buffer; 
        paddedRows = rows; 
        paddedCols = cols;
    } else {
        // Use one of the ImageUtils functions
        switch (paddingChoice) {
            case PaddingChoice::ZERO:
                paddedBuffer = zeroPadImage(*inputImage.buffer, cols, rows, padSize);
                break;
            case PaddingChoice::REPLICATE:
                paddedBuffer = replicatePadImage(*inputImage.buffer, cols, rows, padSize);
                break;
            case PaddingChoice::REFLECT:
                paddedBuffer = reflectPadImage(*inputImage.buffer, cols, rows, padSize);
                break;
            default:
                // Should never happen if we handle all enum cases
                throw std::runtime_error("Unsupported padding choice.");
        }
    }

    // We'll store raw gradient magnitudes in a float vector 
    std::vector<float> gradientMagnitudes(rows * cols, 0.0f);

    // 4. Convolution
    //    We want our final output to match the ORIGINAL image size (rows x cols).
    //    If we physically padded, we can now convolve over the "valid" region
    //    so that the output pixel (i,j) corresponds to the center at (i+padSize, j+padSize) in padded.

    auto getPaddedPixel = [&](int r, int c) -> uint8_t {
        // If no padding was used, we risk out-of-bounds. We'll do a simple check:
        if (r < 0 || r >= paddedRows || c < 0 || c >= paddedCols) {
            return 0; // or skip, or handle differently
        }
        return paddedBuffer[r * paddedCols + c];
    };

    if (isRoberts) {
        // 2x2 operator => we'll basically do:
        // For each valid pixel (i, j) in [0..rows-1], read from padded[i+padSize ...] etc.

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                float sumX = 0.f;
                float sumY = 0.f;

                // The center in padded space if padding is used
                int centerR = i + (paddingChoice == PaddingChoice::NONE ? 0 : padSize);
                int centerC = j + (paddingChoice == PaddingChoice::NONE ? 0 : padSize);

                // Convolve 2x2
                for (int ki = 0; ki < 2; ++ki) {
                    for (int kj = 0; kj < 2; ++kj) {
                        uint8_t pixelVal = getPaddedPixel(centerR + ki, centerC + kj);
                        sumX += pixelVal * gx2[ki][kj];
                        sumY += pixelVal * gy2[ki][kj];
                    }
                }
                float mag = std::sqrt(sumX * sumX + sumY * sumY);
                gradientMagnitudes[i * cols + j] = mag;
            }
        }

    } else {
        // Sobel/Prewitt => 3x3 operator
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                float sumX = 0.f;
                float sumY = 0.f;

                int centerR = i + (paddingChoice == PaddingChoice::NONE ? 0 : padSize);
                int centerC = j + (paddingChoice == PaddingChoice::NONE ? 0 : padSize);

                // Convolve 3x3
                for (int ki = -1; ki <= 1; ++ki) {
                    for (int kj = -1; kj <= 1; ++kj) {
                        uint8_t pixelVal = getPaddedPixel(centerR + ki, centerC + kj);
                        sumX += pixelVal * gx3[ki + 1][kj + 1];
                        sumY += pixelVal * gy3[ki + 1][kj + 1];
                    }
                }
                float mag = std::sqrt(sumX * sumX + sumY * sumY);
                gradientMagnitudes[i * cols + j] = mag;
            }
        }
    }

    // 5. Now we have the gradient magnitudes for each pixel in the original NxM dimension.
    //    If applyThreshold=true, we do a binary map. Otherwise, we scale the range to [0..255].

    std::vector<uint8_t> output(rows * cols, 0);

    if (applyThreshold) {
        // Binary edge map
        for (int i = 0; i < rows * cols; ++i) {
            float mag = gradientMagnitudes[i];
            output[i] = (mag >= thresholdValue) ? 255 : 0;
        }
    } else {
        // Scale to 0..255
        float minVal = gradientMagnitudes[0];
        float maxVal = gradientMagnitudes[0];
        for (int i = 1; i < rows * cols; ++i) {
            if (gradientMagnitudes[i] < minVal) minVal = gradientMagnitudes[i];
            if (gradientMagnitudes[i] > maxVal) maxVal = gradientMagnitudes[i];
        }

        float range = maxVal - minVal;
        if (range < 1e-5) {
            // All magnitudes are ~the same => set everything to 0
            std::fill(output.begin(), output.end(), 0);
        } else {
            for (int i = 0; i < rows * cols; ++i) {
                float normVal = (gradientMagnitudes[i] - minVal) / range; // 0..1
                float scaledVal = normVal * 255.0f;                        // 0..255
                // clamp is a C++17 function in <algorithm>, or write your own
                scaledVal = std::clamp(scaledVal, 0.0f, 255.0f);
                output[i] = static_cast<uint8_t>(scaledVal);
            }
        }
    }

    return output;
}

// Canny Edge Detection ------------------------------------------------------------------------

std::vector<uint8_t> applyCannyEdgeDetection(
    const ImageReadResult& inputImage,
    double lowThreshold,
    double highThreshold,
    double sigma,
    int kernelSize,
    PaddingChoice paddingChoice
) {
    if (!inputImage.meta.isValid() || !inputImage.buffer.has_value()) {
        throw std::invalid_argument("Invalid image or missing buffer!");
    }

    const uint8_t* buffer = inputImage.buffer->data();
    const ImageMetadata& meta = inputImage.meta;
    int rows = meta.height;
    int cols = meta.width;

    // 1. Gaussian Smoothing
    std::vector<uint8_t> smoothedBuffer = applyGaussianFilter(inputImage, kernelSize, sigma);

    // 2. Compute Gradients using Sobel Operator
    int gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    int gy[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    std::vector<float> gradientMagnitude(rows * cols, 0.0f);
    std::vector<float> gradientDirection(rows * cols, 0.0f);

    // Padding the smoothed image
    std::vector<uint8_t> paddedBuffer;
    int padSize = 1;
    int paddedRows = rows + 2 * padSize;
    int paddedCols = cols + 2 * padSize;

    if (paddingChoice == PaddingChoice::NONE) {
        paddedBuffer = smoothedBuffer;
        paddedRows = rows;
        paddedCols = cols;
    } else {
        switch (paddingChoice) {
            case PaddingChoice::ZERO:
                paddedBuffer = zeroPadImage(smoothedBuffer, cols, rows, padSize);
                break;
            case PaddingChoice::REPLICATE:
                paddedBuffer = replicatePadImage(smoothedBuffer, cols, rows, padSize);
                break;
            case PaddingChoice::REFLECT:
                paddedBuffer = reflectPadImage(smoothedBuffer, cols, rows, padSize);
                break;
            default:
                throw std::runtime_error("Unsupported padding choice.");
        }
    }

    // Lambda to get padded pixel safely
    auto getPaddedPixel = [&](int r, int c) -> uint8_t {
        if (r < 0 || r >= paddedRows || c < 0 || c >= paddedCols) {
            return 0;
        }
        return paddedBuffer[r * paddedCols + c];
    };

    // Compute Gradient Magnitude and Direction
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float sumX = 0.f;
            float sumY = 0.f;

            int centerR = i + (paddingChoice == PaddingChoice::NONE ? 0 : padSize);
            int centerC = j + (paddingChoice == PaddingChoice::NONE ? 0 : padSize);

            for (int ki = -1; ki <= 1; ++ki) {
                for (int kj = -1; kj <= 1; ++kj) {
                    uint8_t pixelVal = getPaddedPixel(centerR + ki, centerC + kj);
                    sumX += pixelVal * gx[ki + 1][kj + 1];
                    sumY += pixelVal * gy[ki + 1][kj + 1];
                }
            }

            gradientMagnitude[i * cols + j] = std::sqrt(sumX * sumX + sumY * sumY);
            gradientDirection[i * cols + j] = std::atan2(sumY, sumX) * 180 / M_PI;
        }
    }

    // 3. Non-Maximum Suppression
    std::vector<float> suppressed(rows * cols, 0.0f);       // g_N (x, y)

    for (int i = 1; i < rows - 1; ++i) {
        for (int j = 1; j < cols - 1; ++j) {
            float angle = gradientDirection[i * cols + j];
            angle = std::fmod(angle + 180.0, 180.0);  // Normalize angle between 0 and 180

            float magnitude = gradientMagnitude[i * cols + j];
            float neighbor1 = 0.0f, neighbor2 = 0.0f;

            // Determine neighbors to compare
            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5 && angle <= 180)) {
                neighbor1 = gradientMagnitude[i * cols + (j + 1)];
                neighbor2 = gradientMagnitude[i * cols + (j - 1)];
            } else if (angle >= 22.5 && angle < 67.5) {
                neighbor1 = gradientMagnitude[(i + 1) * cols + (j - 1)];
                neighbor2 = gradientMagnitude[(i - 1) * cols + (j + 1)];
            } else if (angle >= 67.5 && angle < 112.5) {
                neighbor1 = gradientMagnitude[(i + 1) * cols + j];
                neighbor2 = gradientMagnitude[(i - 1) * cols + j];
            } else if (angle >= 112.5 && angle < 157.5) {
                neighbor1 = gradientMagnitude[(i - 1) * cols + (j - 1)];
                neighbor2 = gradientMagnitude[(i + 1) * cols + (j + 1)];
            }

            if (magnitude >= neighbor1 && magnitude >= neighbor2) {
                suppressed[i * cols + j] = magnitude;
            } else {
                suppressed[i * cols + j] = 0.0f;
            }
        }
    }

    // 4. Double Thresholding
    std::vector<uint8_t> output(rows * cols, 0);
    const uint8_t strongEdge = 255;
    const uint8_t weakEdge = 75;

    for (int i = 0; i < rows * cols; ++i) {
        if (suppressed[i] >= highThreshold) {
            output[i] = strongEdge;
        } else if (suppressed[i] >= lowThreshold) {
            output[i] = weakEdge;
        } else {
            output[i] = 0;
        }
    }

    // 5. Edge Tracking by Hysteresis
    auto isStrongNeighbor = [&](int r, int c) -> bool {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (r + dr >= 0 && r + dr < rows && c + dc >= 0 && c + dc < cols) {
                    if (output[(r + dr) * cols + (c + dc)] == strongEdge) {
                        return true;
                    }
                }
            }
        }
        return false;
    };

    for (int i = 1; i < rows - 1; ++i) {
        for (int j = 1; j < cols - 1; ++j) {
            if (output[i * cols + j] == weakEdge) {
                if (isStrongNeighbor(i, j)) {
                    output[i * cols + j] = strongEdge;
                } else {
                    output[i * cols + j] = 0;
                }
            }
        }
    }

    return output;
}