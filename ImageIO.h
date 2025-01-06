#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include <stdint.h> // For uint8_t and size_t

#define HEADER_SIZE 54
#define COLOR_TABLE_SIZE 1024
#define BMP_SIGNATURE_1 0x42
#define BMP_SIGNATURE_2 0x4D

// Struct to hold image metadata (width, height, bit depth)
typedef struct {
    int width;
    int height;
    int bitDepth;
} ImageMetadata;

// Function prototypes for reading and writing images
// - fileName: The name of the file (input/output)
// - header: Array to store the image header (54 bytes for BMP)
// - colorTable: Array to store the color table (if needed)
// - buffer: Array to store the image pixel data
// - meta: Structure to store image metadata (width, height, bit depth)
// - bufferSize: Size of the buffer to store pixel data

int readImage(const char *fileName, uint8_t *header, uint8_t *colorTable, 
                uint8_t *buffer, ImageMetadata *meta, size_t bufferSize);
                
int writeImage(const char *fileName, const uint8_t *header, const uint8_t *colorTable, 
                const uint8_t *buffer, const ImageMetadata *meta, size_t bufferSize);

#endif // IMAGE_IO_H
