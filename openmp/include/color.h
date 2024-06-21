#ifndef __COLOR_H
#define __COLOR_H

#include <stdint.h>

typedef struct {
    // Hue (0-360)
    double h; 

    // Saturation (0-1)
    double s; 

    // Value (brightness, 0-1)
    double v; 
} color_hsv_t;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

#define BLACK   ((color_t){0, 0, 0})
#define RED     ((color_t){255, 0, 0})
#define GREEN   ((color_t){0, 255, 0})
#define BLUE    ((color_t){0, 0, 255})
#define WHITE   ((color_t){255, 255, 255})

/**
 * @brief Add two colors together, the result is the sum of the components modulo 256.
 * 
 * @param c1 first color
 * @param c2 second color
 * @return color_t the sum of the two colors
 */
color_t add_colors(const color_t c1, const color_t c2);

/**
 * @brief Convert a color in HSV format to RGB format.
 * 
 * @param hsv the color in HSV format
 * @return color_t the color in RGB format
 */
color_t hsv_to_rgb(color_hsv_t hsv);

/**
 * @brief Convert a color in RGB format to a hexadecimal string.
 * The string will be in the format "#RRGGBB", so it is 7 characters long.
 * 
 * @param color the color in RGB format
 * @param hex the hexadecimal string
 */
void color_to_hex(color_t color, char* hex);

#endif