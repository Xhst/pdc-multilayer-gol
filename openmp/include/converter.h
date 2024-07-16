#ifndef __CONVERTER_H
#define __CONVERTER_H

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>

/**
 * @brief Converts a string to an unsigned 64-bit integer.
 * 
 * @param str The string to convert
 * @return The converted unsigned 64-bit integer
 */
uint64_t atouint64(const char *str) {
    char *endptr;

    // Reset errno before calling strtoul
    errno = 0;

    // Convert the string to unsigned long
    unsigned long result = strtoul(str, &endptr, 10);

    // Check for errors or invalid input
    if (errno == ERANGE || result > UINT32_MAX || endptr == str || *endptr != '\0') {
        return 0;
    }

    return (uint64_t) result;
}

#endif