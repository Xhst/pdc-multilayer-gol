#include "image.h"

#include <png.h>
#include <stdlib.h>

void write_png_file(const char* filename, uint64_t width, uint64_t height, uint8_t* buffer) {
    FILE *fp = fopen(filename, "wb");
    if(!fp) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        abort();
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fprintf(stderr, "Could not create write struct\n");
        abort();
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        fprintf(stderr, "Could not create info struct\n");
        abort();
    }

    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "Error during png creation\n");
        abort();
    }

    png_init_io(png, fp);

    // 8-bit depth
    const int depth = 8;

    // RGB format.
    png_set_IHDR(
        png,
        info,
        width, 
        height,
        depth,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    png_bytep rows[height];

    for(uint64_t y = 0; y < height; y++) {
        rows[y] = &buffer[y * width * 3];
    }

    png_write_image(png, rows);
    png_write_end(png, NULL);

    fclose(fp);

    png_destroy_write_struct(&png, &info);
}