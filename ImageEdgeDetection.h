#ifndef IMAGE_EDGE_DETECTION_H
#define IMAGE_EDGE_DETECTION_H

#include <vector>
#include <cstdint>
#include "ImageIO.h" 
#include "ImageUtils.h"

/**
 * @brief Applies a gradient-based edge detection with optional thresholding & padding.
 *
 * @param inputImage     The input image result (must contain a valid buffer).
 * @param kernelChoice   Which kernel to use (SOBEL, PREWITT, ROBERTS).
 * @param applyThreshold If true, we produce a binary edge map; false => gradient map.
 * @param thresholdValue The threshold used if applyThreshold=true.
 * @param paddingChoice  Which padding method to use (NONE, ZERO, REPLICATE, REFLECT).
 * @return A std::vector<uint8_t> representing the resulting image (same width & height as input).
 */
std::vector<uint8_t> applyGradientEdgeDetection(
    const ImageReadResult& inputImage,
    KernelChoice kernelChoice,
    bool applyThreshold,
    double thresholdValue,
    PaddingChoice paddingChoice
);

std::vector<uint8_t> applyCannyEdgeDetection(
    const ImageReadResult& inputImage,
    double lowThreshold,
    double highThreshold,
    double sigma,
    int kernelSize,
    PaddingChoice paddingChoice
);



#endif