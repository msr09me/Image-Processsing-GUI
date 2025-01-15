#include "ImageIO.h"
#include <fstream>
#include <cstring>
#include <stdexcept>

// Helper function to log messages

void log(LogLevel level, const std::string &message) {
    const char *prefix = (level == INFO) ? "[INFO]" : (level == WARNING) ? "[WARNING]" : "[ERROR]";
    std::cerr << prefix << " " << message << std::endl;
}

// Detect file format based on the signature
std::string detectFileFormat(const std::string &filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) return "unknown";

    uint8_t signature[2];
    file.read(reinterpret_cast<char *>(signature), 2);

    if (signature[0] == 'B' && signature[1] == 'M') return "BMP";
    // Add detection logic for other formats as needed
    return "unknown";
}

// Read image and return buffer
ImageReadResult readImage(const std::string &filePath) {
    log(INFO, "Opening file: " + filePath);

    // Open file
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        log(ERROR, "Failed to open file: " + filePath);
        return {std::nullopt, {}};
    }

    // Read the header
    std::vector<uint8_t> header(HEADER_SIZE);
    file.read(reinterpret_cast<char *>(header.data()), HEADER_SIZE);
    if (!file) {
        log(ERROR, "Failed to read image header.");
        return {std::nullopt, {}};
    }

    // Validate BMP signature
    if (header[0] != 'B' || header[1] != 'M') {
        log(ERROR, "File is not a valid BMP file.");
        return {std::nullopt, {}};
    }

    // Extract metadata
    ImageMetadata meta;
    meta.width = *reinterpret_cast<int *>(&header[18]);
    meta.height = *reinterpret_cast<int *>(&header[22]);
    meta.bitDepth = *reinterpret_cast<int *>(&header[28]);

    // To avoid crash because of a larze image

    if (meta.width > 10000 || meta.height > 10000) { // Example limits
        log(ERROR, "Image dimensions exceed reasonable limits. Width: " + std::to_string(meta.width) +
                       ", Height: " + std::to_string(meta.height));
        return {std::nullopt, {}};
    }

    // Print the meta data

    log(INFO, "Image Metadata: Width=" + std::to_string(meta.width) +
                  ", Height=" + std::to_string(meta.height) +
                  ", Bit Depth=" + std::to_string(meta.bitDepth));



    // Validate metadata
    if (!meta.isValid()) {
        log(ERROR, "Invalid metadata extracted from image header.");
        return {std::nullopt, {}};
    }

    if (meta.bitDepth != 8 && meta.bitDepth != 24) {
        log(ERROR, "Unsupported bit depth: " + std::to_string(meta.bitDepth));
        return {std::nullopt, {}};
    }

    // Read the color table if applicable
    std::vector<uint8_t> colorTable;
    log(INFO, "Reading color table for 8-bit BMP.");
    if (meta.bitDepth <= 8) {
        colorTable.resize(COLOR_TABLE_SIZE);
        file.read(reinterpret_cast<char *>(colorTable.data()), COLOR_TABLE_SIZE);
        if (!file) {
            log(ERROR, "Failed to read color table.");
            return {std::nullopt, {}};
        }
    }

    // Calculate buffer size
    size_t bufferSize = static_cast<size_t>(meta.width) * meta.height * (meta.bitDepth / 8);
    if (bufferSize == 0 || bufferSize > 10 * 1024 * 1024) { // Example limit: 10 MB
        log(ERROR, "Buffer size calculation failed or exceeds limits."+ std::to_string(bufferSize));
        return {std::nullopt, {}};
    }

    // Read pixel data
    std::vector<uint8_t> buffer(bufferSize);
    file.read(reinterpret_cast<char *>(buffer.data()), bufferSize);
    if (!file) {
        log(ERROR, "Failed to read pixel data.");
        return {std::nullopt, {}};
    }

    log(INFO, "Image data successfully read.");
    return {buffer, colorTable, header, meta};
}

// Write image to file
bool writeImage(const std::string &filePath, const ImageReadResult &result) {

    log(INFO, "Writing image to: " + filePath);

    // Use references to avoid extra memory allocation
    const auto &buffer = result.buffer;
    const auto &colorTable = result.colorTable;
    const auto &header = result.header;
    const auto &meta = result.meta;

    // Validate metadata
    if (!meta.isValid()) {
        log(ERROR, "Invalid metadata. Cannot write image.");
        return false;
    }

    // Validate buffer size
    size_t rowSize = ((meta.width * meta.bitDepth + 31) / 32) * 4; // Padded row size
    size_t expectedSize = rowSize * meta.height;
    if (buffer->size() != expectedSize) {
        log(ERROR, "Buffer size mismatch. Expected: " + std::to_string(expectedSize) +
                       ", Actual: " + std::to_string(buffer->size()));
        return false;
    }

    // Open the file for writing
    std::ofstream file(filePath, std::ios::binary);
    if (!file) {
        log(ERROR, "Failed to open file for writing: " + filePath);
        return false;
    }

    // Write the updated header
    log(INFO, "Writing header...");
    file.write(reinterpret_cast<const char *>(header.data()), HEADER_SIZE);
    if (!file) {
        log(ERROR, "Failed to write image header.");
        return false;
    }

    // Write the color table if applicable
    if (meta.bitDepth <= 8 && !colorTable.empty()) {
        log(INFO, "Writing color table...");
        file.write(reinterpret_cast<const char *>(colorTable.data()), colorTable.size());
        if (!file) {
            log(ERROR, "Failed to write color table.");
            return false;
        }
    }

    // Write pixel data row by row with padding
    log(INFO, "Writing pixel data...");
    for (int y = 0; y < meta.height; ++y) {
        size_t rowStart = y * meta.width * (meta.bitDepth / 8);
        file.write(reinterpret_cast<const char *>(&(*buffer)[rowStart]), meta.width * (meta.bitDepth / 8));

        // Add padding
        int paddingBytes = rowSize - meta.width * (meta.bitDepth / 8);
        if (paddingBytes > 0) {
            uint8_t padding[3] = {0, 0, 0}; // Padding bytes
            file.write(reinterpret_cast<const char *>(padding), paddingBytes);
        }
    }

    if (!file) {
        log(ERROR, "Failed to write pixel data.");
        return false;
    }

    file.flush();
    file.close();
    log(INFO, "Image successfully written to: " + filePath);
    return true;
}
