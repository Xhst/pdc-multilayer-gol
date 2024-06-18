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

/**
 * @brief Initializes the grid with the given density.
 * 
 * @param gol The game of life structure
 * @param density The density of the grid
 */
void init_grid(const gol_t* gol, float density);

/**
 * @brief Counts the number of alive neighbors of a cell.
 * 
 * @param gol The game of life structure
 * @param i The row of the cell
 * @param j The column of the cell
 * @return The number of alive neighbors
 */
uint8_t count_alive_neighbors(const gol_t* gol, uint32_t i, uint32_t j);

/**
 * @brief Performs one step of the game of life.
 * 
 * @param gol The game of life structure
 */
void step(const gol_t* gol);

/**
 * @brief Swaps the current and next grids.
 * 
 * @param gol The game of life structure
 */
void swap_grids(gol_t* gol);

#endif