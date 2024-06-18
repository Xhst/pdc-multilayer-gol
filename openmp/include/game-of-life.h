#ifndef __GAME_OF_LIFE_H
#define __GAME_OF_LIFE_H

#include <stdint.h>

typedef uint8_t cell_t;

typedef struct {
    cell_t* current;
    cell_t* next;
    uint32_t size;
} gol_t;

/**
 * @brief Initializes the game of life's grid with the given density.
 * 
 * @param gol The game of life structure
 * @param grid_size The size of the grid
 * @param density The density of the grid
 */
void init_gol(gol_t *gol, uint32_t grid_size, uint32_t seed, float density);

#endif