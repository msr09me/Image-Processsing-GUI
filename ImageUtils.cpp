#include "ImageUtils.h"
#include <algorithm> // for std::clamp

std::vector<uint8_t> replicatePadImage(
    const std::vector<uint8_t>& inputBuffer,
    int width,
    int height,
    int padSize
) {
    // 1. Calculate new dimensions
    int newWidth  = width  + 2 * padSize;
    int newHeight = height + 2 * padSize;

    // 2. Create output buffer filled with 0 initially (or any default)
    std::vector<uint8_t> outputBuffer(newWidth * newHeight, 0);

    // 3. A helper lambda to read from the input with replicate boundary
    auto getPixel = [&](int r, int c) -> uint8_t {
        // clamp row/col to [0, height-1] and [0, width-1]
        r = std::clamp(r, 0, height - 1);
        c = std::clamp(c, 0, width - 1);
        return inputBuffer[r * width + c];
    };

    // 4. Fill the output buffer
    //    For each pixel (R, C) in the NEW (padded) coordinates,
    //    map it back to input with replicate padding
    for (int R = 0; R < newHeight; ++R) {
        for (int C = 0; C < newWidth; ++C) {
            // The corresponding row/col in the original image
            int originalRow = R - padSize;
            int originalCol = C - padSize;

            // Retrieve the pixel with replicate boundary
            uint8_t value = getPixel(originalRow, originalCol);

            // Place into new buffer
            outputBuffer[R * newWidth + C] = value;
        }
    }

    return outputBuffer;
}

std::vector<uint8_t> zeroPadImage(
    const std::vector<uint8_t>& inputBuffer,
    int width,
    int height,
    int padSize
) {
    // 1. Calculate new dimensions
    int newWidth  = width  + 2 * padSize;
    int newHeight = height + 2 * padSize;

    // 2. Create output buffer filled with 0
    std::vector<uint8_t> outputBuffer(newWidth * newHeight, 0);

    // 3. Copy the original image into the center
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            // Map the original pixel to the new buffer
            int newR = r + padSize;
            int newC = c + padSize;
            outputBuffer[newR * newWidth + newC] = inputBuffer[r * width + c];
        }
    }

    // Borders remain 0
    return outputBuffer;
}

std::vector<uint8_t> reflectPadImage(
    const std::vector<uint8_t>& inputBuffer,
    int width,
    int height,
    int padSize
) {
    // 1. Calculate new dimensions
    int newWidth  = width  + 2 * padSize;
    int newHeight = height + 2 * padSize;

    // 2. Create output buffer (fill with 0 or some default)
    std::vector<uint8_t> outputBuffer(newWidth * newHeight, 0);

    // 3. Helper to read original pixels with reflection
    auto getPixelReflect = [&](int r, int c) -> uint8_t {
        // If r < 0, reflect to + side: e.g., -1 => 1, -2 => 2, etc.
        // One approach is: r = -r if out of bounds, or mirror around edges.
        // A simpler approach is to use modulo with reflection logic
        // but let's do a direct clamp for demonstration.

        if (r < 0)          r = -r - 1;       // reflect above top
        if (r >= height)    r = 2*height - r - 1; // reflect below bottom
        if (c < 0)          c = -c - 1;       // reflect left
        if (c >= width)     c = 2*width - c - 1;  // reflect right

        return inputBuffer[r * width + c];
    };

    // 4. Fill output using reflection
    for (int R = 0; R < newHeight; ++R) {
        for (int C = 0; C < newWidth; ++C) {
            int originalRow = R - padSize;
            int originalCol = C - padSize;

            uint8_t value = getPixelReflect(originalRow, originalCol);
            outputBuffer[R * newWidth + C] = value;
        }
    }

    return outputBuffer;
}
