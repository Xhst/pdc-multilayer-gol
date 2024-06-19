#ifndef __ML_GOL_H
#define __ML_GOL_H

#include <stdint.h>

#include "game-of-life.h"

typedef uint8_t cell_t;

typedef struct {
    gol_t* layers;
    uint32_t num_layers;
    cell_t* combined;
    cell_t* dependent;
    uint32_t grid_size;
} ml_gol_t;

/**
 * @brief Function to start the multilayer game of life.
 * 
 * @param grid_size Size of the grid
 * @param num_layers Number of layers
 * @param num_steps Number of steps
 * @param seed Seed for the random number generator
 */
void start_game(uint32_t grid_size, uint32_t num_layers, uint32_t num_steps, uint32_t seed);

/**
 * @brief Initializes the multilayer game of life structure.
 * 
 * @param ml_gol The multilayer game of life structure
 * @param grid_size Size of the grid
 * @param num_layers Number of layers
 * @param seed Seed for the random number generator
 */
void init_ml_gol(ml_gol_t* ml_gol, uint32_t grid_size, uint32_t num_layers, uint32_t seed);

/**
 * @brief Calculates the combined grid from the layers of the multilayer game of life.
 * 
 * @param ml_gol The multilayer game of life structure
 */
void calculate_combined(const ml_gol_t* ml_gol);

/**
 * @brief Calculates the dependent grid from the layers of the multilayer game of life.
 * 
 * @param ml_gol The multilayer game of life structure
 */
void calculate_dependent(const ml_gol_t* ml_gol);

/**
 * @brief Frees the memory allocated for the multilayer game of life structure.
 * 
 * @param ml_gol The multilayer game of life structure
 */
void free_ml_gol(ml_gol_t* ml_gol);

#endif