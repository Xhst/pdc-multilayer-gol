#ifndef __ML_GOL_H
#define __ML_GOL_H

#include <stdint.h>

#include "game_of_life.h"
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
    uint64_t num_layers;
    color_t* combined;
    color_t* dependent;
    uint64_t grid_size;
} ml_gol_t;

/**
 * @brief Function to start the multilayer game of life.
 * 
 * @param grid_size Size of the grid
 * @param num_layers Number of layers
 * @param num_steps Number of steps
 * @param use_png Flag to indicate if PNG files should be created
 * @param density Density of the grid
 * @param seed Seed for the random number generator
 */
void start_game(uint64_t grid_size, uint64_t num_layers, uint64_t num_steps, bool use_png, float density, uint64_t seed);

/**
 * @brief Initializes the multilayer game of life structure.
 * 
 * @param ml_gol The multilayer game of life structure
 * @param grid_size Size of the grid
 * @param num_layers Number of layers
 * @param density Density of the grid
 * @param seed Seed for the random number generator
 */
void init_ml_gol(ml_gol_t* ml_gol, uint64_t grid_size, uint64_t num_layers, float density, uint64_t seed);

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
void create_png_for_step(const ml_gol_t* ml_gol, uint64_t step);

/**
 * @brief Creates a PNG file for the given grid.
 * 
 * @param grid The grid
 * @param grid_size The size of the grid
 * @param step The step number
 * @param folder The folder to save the PNG file
 */
void create_png_for_grid(const color_t* grid, uint64_t grid_size, uint64_t step, const char* folder);

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
void write_png_file(const char* filename, uint64_t width, uint64_t height, uint8_t* buffer);

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
color_t get_color_for_layer(uint64_t layer, const uint64_t num_layers);

/**
 * @brief Prints the colors for the layers of the multilayer game of life.
 * 
 * @param ml_gol The multilayer game of life structure
 */
void print_layers_colors(const ml_gol_t* ml_gol);

/**
 * @brief Counts the number of alive neighbors for the given cell in the multilayer game of life.
 * 
 * @param ml_gol The multilayer game of life structure
 * @param i The row index
 * @param j The column index
 * @return The number of alive neighbors
 */
uint8_t count_dependent_alive_neighbors(const ml_gol_t* ml_gol, uint64_t i, uint64_t j);

/**
 * @brief Frees the memory allocated for the multilayer game of life structure.
 * 
 * @param ml_gol The multilayer game of life structure
 */
void free_ml_gol(ml_gol_t* ml_gol);

#endif