#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <vector>
#include <cstdint>

// 1. Kernel choice enum
enum class KernelChoice {
    SOBEL = 1,
    PREWITT,
    ROBERTS
};

// 2. Padding choice enum
enum class PaddingChoice {
    NONE = 0,
    ZERO,
    REPLICATE,
    REFLECT
};

/**
 * @brief Creates a new image buffer with replicate padding.
 * 
 * @param inputBuffer  The original image buffer (grayscale).
 * @param width        Original image width.
 * @param height       Original image height.
 * @param padSize      The number of pixels to pad on each side.
 * @return A new buffer (std::vector<uint8_t>) with dimensions:
 *         (height + 2*padSize) x (width + 2*padSize).
 */
std::vector<uint8_t> replicatePadImage(
    const std::vector<uint8_t>& inputBuffer,
    int width,
    int height,
    int padSize
);

/**
 * @brief Creates a new image buffer with zero padding.
 *
 * @param inputBuffer  The original image buffer.
 * @param width        Original image width.
 * @param height       Original image height.
 * @param padSize      The number of pixels to pad on each side.
 * @return A new buffer with dimensions
 *         (height + 2*padSize) x (width + 2*padSize), filled with 0 on borders.
 */
std::vector<uint8_t> zeroPadImage(
    const std::vector<uint8_t>& inputBuffer,
    int width,
    int height,
    int padSize
);

/**
 * @brief Creates a new image buffer with mirror (reflect) padding.
 *
 * @param inputBuffer  The original image buffer.
 * @param width        Original image width.
 * @param height       Original image height.
 * @param padSize      The number of pixels to pad on each side.
 * @return A new buffer with mirror reflection on the borders.
 */
std::vector<uint8_t> reflectPadImage(
    const std::vector<uint8_t>& inputBuffer,
    int width,
    int height,
    int padSize
);


#endif
