#ifndef __ML_GOL_H
#define __ML_GOL_H

#include <stdint.h>
#include <stdbool.h>

#include "color.h"

/**
 * @brief Structure to represent the multilayer game of life.
 * 
 * The multilayer game of life is represented by an array of game of life structures (layers).
 * The layers represent different instances of the game of life, each with standard rules.
 * The combined grid represents the combined state of all the layers, while the dependent grid represents a dependent state based on the layers.
 * The grid_size represents the size of the grids (layers, combined and dependent).
 * The current and next field represent the multilayer game of life as a matrix of arrays. Each element (cell) of the matrix is an array of bool, one for each layer.
 */
typedef struct {
    bool* current;
    bool* next;
    uint64_t num_layers;
    color_t* layers_colors;
    color_t* combined;
    color_t* dependent;
    uint64_t grid_size;
} ml_gol_t;

/**
 * @brief Function to start the multilayer game of life on GPU.
 * 
 * @param grid_size Size of the grid
 * @param num_layers Number of layers
 * @param num_steps Number of steps
 * @param use_png Flag to indicate if PNG files should be created
 * @param density Density of the grid
 * @param seed Seed for the random number generator
 */
void start_game_on_cuda(uint64_t grid_size, uint64_t num_layers, uint64_t num_steps, bool use_png, float density, uint64_t seed);

/**
 * @brief Initializes the multilayer game of life structure on CPU and GPU using Unified Memory.
 * 
 * @param ml_gol The multilayer game of life structure
 * @param grid_size Size of the grid
 * @param num_layers Number of layers
 * @param create_png Flag to indicate if PNG files should be created
 * @param density Density of the grid
 * @param seed Seed for the random number generator
 */
void init_ml_gol_managed(ml_gol_t* ml_gol, uint64_t grid_size, uint64_t num_layers, float density, uint64_t seed);

/**
 * @brief Initializes the multilayer game of life structure.
 * 
 * @param ml_gol The multilayer game of life structure
 * @param grid_size Size of the grid
 * @param num_layers Number of layers
 * @param create_png Flag to indicate if PNG files should be created
 * @param density Density of the grid
 * @param seed Seed for the random number generator
 */
void init_ml_gol(ml_gol_t* ml_gol, uint64_t grid_size, uint64_t num_layers, float density, uint64_t seed);

/**
 * @brief Allocates and copies ml_gol structure from host to device
 * 
 * @param d_ml_gol The structure on the device
 * @param ml_gol The already allocated structure on the host
*/
void init_ml_gol_to_device(const ml_gol_t* ml_gol, bool** d_current, bool** d_next, color_t** d_layers_colors, color_t** d_combined, color_t** d_dependent);

void update_d_ml_gol(const ml_gol_t* ml_gol, bool** d_current, bool** d_next, color_t** d_combined, color_t** d_dependent);

void copy_back_to_host(const ml_gol_t* ml_gol, bool** d_current, bool** d_next, color_t** d_combined, color_t** d_dependent);

void launch_kernel_each_step(ml_gol_t* ml_gol, bool** d_current, bool** d_next, color_t** d_layers_colors, color_t** d_combined, color_t** d_dependent, uint64_t num_steps, dim3 gridDim, dim3 blockDim);

/**
 * @brief Fills ghost cells in all layers of the multilayer game of life
 * 
 * @param current The current grid
 * @param grid_size The size of the grid
 * @param num_layers The number of layers
*/
__host__ __device__ void fill_ghost_cells(bool* current, uint64_t grid_size, uint64_t num_layers);

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
 * @brief Swaps the current and next grids.
 * 
 * @param gol The game of life structure
 */
void swap_grids(ml_gol_t* ml_gol);

/**
 * @brief Swaps the current and next grids on the device
 * 
 * @param d_current The current grid
 * @param d_next The next grid
 */
__device__ void swap_grids_device(bool** d_current, bool** d_next);

/**
 * @brief A function to calculate the grid idx (layer independent) from host or device
 * 
 * @param ml_gol The game of life structure
 * @param i The row of the cell
 * @param j The column of the cell
 */
__host__ __device__ size_t idx(const ml_gol_t* ml_gol, uint64_t i, uint64_t j);

/**
 * @brief A function to calculate the grid idx (layer independent) from host or device for a flat cell structure
 * 
 * @param grid_size The size of the grid
 * @param num_layers The number of layers
 * @param i The row of the cell
 * @param j The column of the cell
 * @param layer The layer of the cell
 */
__host__ __device__ size_t idx_flat(uint64_t grid_size, uint64_t num_layers, uint64_t i, uint64_t j, uint64_t layer);

/**
 * @brief Counts the number of alive neighbors of a cell in a layer.
 * 
 * @param current The current grid
 * @param grid_size The size of the grid
 * @param num_layers The number of layers
 * @param i The row of the cell
 * @param j The column of the cell
 * @param layer The layer to analyze
 * @return The number of alive neighbors
 */
__device__ uint8_t count_layer_alive_neighbors(bool* current, uint64_t grid_size, uint64_t num_layers, uint64_t i, uint64_t j, uint64_t layer);

/**
 * @brief The kernel to calculate ONE STEP pf the multilayer GoL on the device 
 * 
 * @param d_current The current grid
 * @param d_next The next grid
 * @param d_layers_colors The colors for the layers
 * @param d_combined The combined grid
 * @param d_dependent The dependent grid
 * @param grid_size The size of the grid
 * @param num_layers The number of layers
 */
__global__ void ml_gol_kernel_one_step(bool* d_current, bool* d_next, color_t* d_layers_colors, color_t* d_combined, color_t* d_dependent, uint64_t grid_size, uint64_t num_layers);

/**
 * @brief The kernel to calculate @param num_steps of multilayer GoL on the device 
 * 
 * @param d_current The current grid
 * @param d_next The next grid
 * @param d_layers_colors The colors for the layers
 * @param d_combined The combined grid
 * @param d_dependent The dependent grid
 * @param grid_size The size of the grid
 * @param num_layers The number of layers
 * @param num_steps The number of steps to perform
 */
__global__ void ml_gol_kernel_n_steps(bool* d_current, bool* d_next, color_t* d_layers_colors, color_t* d_combined, color_t* d_dependent, uint64_t grid_size, uint64_t num_layers, uint64_t num_steps);


#endif