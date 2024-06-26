#ifndef __GAME_OF_LIFE_H
#define __GAME_OF_LIFE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Structure to represent the game of life.
 * 
 * The game of life is represented by two grids: the current grid and the next grid.
 * The current grid represents the current state of the game, while the next grid represents the next state of the game.
 * Each grid is represented as an array of boolean values, where true represents an alive cell and false represents a dead cell.
 * The variable size represents the size of the grids.
 */
typedef struct {
    bool* current;
    bool* next;
    uint32_t size;
} gol_t;

/**
 * @brief Initializes the game of life's grid with the given density.
 * 
 * @param gol The game of life structure
 * @param grid_size The size of the grid
 * @param density The density of the grid
 */
void init_gol(gol_t *gol, uint32_t grid_size, float density);

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

/**
 * @brief Fills the ghost cells of the grid.
 * 
 * @param gol The game of life structure
 */
void fill_ghost_cells(const gol_t* gol);

/**
 * @brief Frees the memory allocated for the game of life structure.
 * 
 * @param gol The game of life structure
 */
void free_gol(gol_t* gol);

/**
 * @brief Returns the index of a cell in the grid.
 * 
 * @param gol The game of life structure
 * @param i The row of the cell
 * @param j The column of the cell
 * @return size_t The index of the cell
 */
static inline size_t idx(const gol_t* gol, uint32_t i, uint32_t j) {
    return i * gol->size + j;
}

#endif