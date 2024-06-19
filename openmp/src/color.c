#include "color.h"

#include <math.h>

color_t add_colors(const color_t c1, const color_t c2) {
    color_t result;
    result.r = (c1.r + c2.r) % 256;
    result.g = (c1.g + c2.g) % 256;
    result.b = (c1.b + c2.b) % 256;
    return result;
}

color_t hsv_to_rgb(color_hsv_t hsv) {
    color_t rgb;
    double c = hsv.v * hsv.s;
    double x = c * (1 - fabs(fmod(hsv.h / 60.0, 2) - 1));
    double m = hsv.v - c;

    double r, g, b;
    if (hsv.h >= 0 && hsv.h < 60) {
        r = c;
        g = x;
        b = 0;
    } else if (hsv.h >= 60 && hsv.h < 120) {
        r = x;
        g = c;
        b = 0;
    } else if (hsv.h >= 120 && hsv.h < 180) {
        r = 0;
        g = c;
        b = x;
    } else if (hsv.h >= 180 && hsv.h < 240) {
        r = 0;
        g = x;
        b = c;
    } else if (hsv.h >= 240 && hsv.h < 300) {
        r = x;
        g = 0;
        b = c;
    } else {
        r = c;
        g = 0;
        b = x;
    }

    rgb.r = (uint8_t)((r + m) * 255);
    rgb.g = (uint8_t)((g + m) * 255);
    rgb.b = (uint8_t)((b + m) * 255);

    return rgb;
}