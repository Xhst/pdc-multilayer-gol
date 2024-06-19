#ifndef __IMAGE_H
#define __IMAGE_H

#include <stdint.h>

/**
 * @brief Writes a PNG file with the given buffer.
 * 
 * @param filename The name of the file
 * @param width The width of the image
 * @param height The height of the image
 * @param buffer The buffer with the image data
 */
void write_png_file(const char* filename, uint32_t width, uint32_t height, uint8_t* buffer);

#endif