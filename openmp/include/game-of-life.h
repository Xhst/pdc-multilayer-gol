#ifndef __GAME_OF_LIFE_H
#define __GAME_OF_LIFE_H

#include <stdint.h>

typedef uint8_t cell_t;

typedef struct {
    cell_t* current;
    cell_t* next;
    uint32_t size;
} gol_t;

#endif