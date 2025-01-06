#include <cstdio>
#include <cstring>
#include "ImageIO.h"

// Read image with error handling
int readImage(const char *fileName, uint8_t *header, uint8_t *colorTable, uint8_t *buffer, ImageMetadata *meta, size_t bufferSize) {
    FILE *file = fopen(fileName, "rb");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    // Check for BMP signature (first two bytes)
    uint8_t signature[2];
    if (fread(signature, sizeof(uint8_t), 2, file) != 2 || signature[0] != BMP_SIGNATURE_1 || signature[1] != BMP_SIGNATURE_2) {
        fprintf(stderr, "Error: Unsupported file format or corrupted file.\n");
        fclose(file);
        return -1;
    }

    // Seek back to the beginning after reading signature
    fseek(file, 0, SEEK_SET);

    // Read the header and validate its size
    if (fread(header, sizeof(uint8_t), HEADER_SIZE, file) != HEADER_SIZE) {
        fprintf(stderr, "Error: Failed to read the full header or corrupted file.\n");
        fclose(file);
        return -1;
    }

    // Extract metadata from the header
    meta->width = *(int *)&header[18];
    meta->height = *(int *)&header[22];
    meta->bitDepth = *(int *)&header[28];

    // Check if the image dimensions are valid
    if (meta->width <= 0 || meta->height <= 0) {
        fprintf(stderr, "Error: Invalid image dimensions.\n");
        fclose(file);
        return -1;
    }

    // Handle color table based on bit depth
    if (meta->bitDepth <= 8) {
        if (fread(colorTable, sizeof(uint8_t), COLOR_TABLE_SIZE, file) != COLOR_TABLE_SIZE) {
            fprintf(stderr, "Error: Failed to read the color table or corrupted file.\n");
            fclose(file);
            return -1;
        }
    }

    // Check buffer size if it's passed
    if (buffer != nullptr && bufferSize < static_cast<size_t>(meta->width * meta->height * (meta->bitDepth / 8))) {
        fprintf(stderr, "Error: Insufficient buffer size for the image data.\n");
        fclose(file);
        return -1;
    }

    // Read the image data into the buffer
    if (buffer != nullptr && fread(buffer, sizeof(uint8_t), bufferSize, file) != bufferSize) {
        fprintf(stderr, "Error: Failed to read the image data.\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

// Write image with error handling
int writeImage(const char *fileName, const uint8_t *header, const uint8_t *colorTable, const uint8_t *buffer, const ImageMetadata *meta, size_t bufferSize) {
    FILE *file = fopen(fileName, "wb");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    // Write the header
    if (fwrite(header, sizeof(uint8_t), HEADER_SIZE, file) != HEADER_SIZE) {
        fprintf(stderr, "Error: Failed to write the header.\n");
        fclose(file);
        return -1;
    }

    // Write the color table if applicable
    if (meta->bitDepth <= 8) {
        if (fwrite(colorTable, sizeof(uint8_t), COLOR_TABLE_SIZE, file) != COLOR_TABLE_SIZE) {
            fprintf(stderr, "Error: Failed to write the color table.\n");
            fclose(file);
            return -1;
        }
    }

    // Write the image data
    if (fwrite(buffer, sizeof(uint8_t), bufferSize, file) != bufferSize) {
        fprintf(stderr, "Error: Failed to write the image data.\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}
