#ifndef __ML_GOL_H
#define __ML_GOL_H

#include <stdint.h>

/**
 * @brief Function to start the multilayer game of life.
 * 
 * @param grid_size Size of the grid
 * @param num_layers Number of layers
 * @param num_steps Number of steps
 * @param seed Seed for the random number generator
 */
void start_game(uint32_t grid_size, uint32_t num_layers, uint32_t num_steps, uint32_t seed);

#endif