#ifndef __ML_GOL_H
#define __ML_GOL_H

#include <stdint.h>

#include "game-of-life.h"
#include "color.h"

/**
 * @brief Structure to represent the multilayer game of life.
 * 
 * The multilayer game of life is represented by an array of game of life structures (layers).
 * The layers represent different instances of the game of life, each with standard rules.
 * The combined grid represents the combined state of all the layers, while the dependent grid represents a dependent state based on the layers.
 * The grid_size represents the size of the grids (layers, combined and dependent).
 */
typedef struct {
    gol_t* layers;
    uint32_t num_layers;
    color_t* combined;
    color_t* dependent;
    uint32_t grid_size;
} ml_gol_t;

/**
 * @brief Function to start the multilayer game of life.
 * 
 * @param grid_size Size of the grid
 * @param num_layers Number of layers
 * @param num_steps Number of steps
 * @param density Density of the grid
 * @param seed Seed for the random number generator
 */
void start_game(uint32_t grid_size, uint32_t num_layers, uint32_t num_steps, float density, uint32_t seed);

/**
 * @brief Initializes the multilayer game of life structure.
 * 
 * @param ml_gol The multilayer game of life structure
 * @param grid_size Size of the grid
 * @param num_layers Number of layers
 * @param density Density of the grid
 * @param seed Seed for the random number generator
 */
void init_ml_gol(ml_gol_t* ml_gol, uint32_t grid_size, uint32_t num_layers, float density, uint32_t seed);

/**
 * @brief Calculates the combined grid from the layers of the multilayer game of life.
 * 
 * @param ml_gol The multilayer game of life structure
 */
void calculate_combined(const ml_gol_t* ml_gol);

/**
 * @brief Creates a PNG file for the given step of the multilayer game of life.
 * 
 * @param ml_gol The multilayer game of life structure
 * @param step The step number
 */
void create_png_for_step(const ml_gol_t* ml_gol, const uint32_t step);

/**
 * @brief Resets the combined and dependent grids to black.
 * 
 * @param ml_gol The multilayer game of life structure
 */
void reset_combined_and_dependent(ml_gol_t* ml_gol);

/**
 * @brief Writes the given buffer to a PNG file.
 * 
 * @param filename The name of the file
 * @param width The width of the image
 * @param height The height of the image
 * @param buffer The buffer containing the image data
 */
void write_png_file(const char* filename, uint32_t width, uint32_t height, uint8_t* buffer);

/**
 * @brief Calculates the dependent grid from the layers of the multilayer game of life.
 * 
 * @param ml_gol The multilayer game of life structure
 */
void calculate_dependent(const ml_gol_t* ml_gol);

/**
 * @brief Gets the color for the given layer.
 * 
 * @param layer The layer number
 * @param num_layers The total number of layers
 * @return The color for the layer
 */
color_t get_color_for_layer(uint32_t layer, const uint32_t num_layers);

/**
 * @brief Frees the memory allocated for the multilayer game of life structure.
 * 
 * @param ml_gol The multilayer game of life structure
 */
void free_ml_gol(ml_gol_t* ml_gol);

#endif