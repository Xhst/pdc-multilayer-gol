/**
 * @file main.c
 * @brief Main file for the OpenMP implementation of the Game of Life
 * 
 * How to compile:
 * move to the openmp directory and run 'make' command.
 * Check the Makefile for more details.
 * 
 * How to run (from the openmp directory):
 * ./bin/multilayer-game-of-life <grid_size> <num_layers> <num_steps> <seed>
 * The parameters are optional, if not provided, the default values are used.
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "converter.h"
#include "ml-gol.h"

#define DEFAULT_GRID_SIZE 128
#define DEFAULT_NUM_LAYERS 3
#define DEFAULT_DENSITY 0.3
#define DEFAULT_NUM_STEPS 64

int main(int argc, char *argv[]) {
    
    uint32_t grid_size = DEFAULT_GRID_SIZE;
    uint32_t num_layers = DEFAULT_NUM_LAYERS;
    uint32_t num_steps = DEFAULT_NUM_STEPS;
    float density = DEFAULT_DENSITY;
    uint32_t seed = time(NULL);

    if (argc > 1) {
        grid_size = atouint32(argv[1]);
    }

    if (argc > 2) {
        num_layers = atouint32(argv[2]);
    }

    if (argc > 3) {
        num_steps = atouint32(argv[3]);
    }

    if (argc > 4) {
        density = atof(argv[4]);
    }

    if (argc > 5) {
        seed = atouint32(argv[4]);
    }

    if (grid_size == 0 || num_layers == 0 || num_steps == 0) {
        fprintf(stderr, "Invalid input\n");
        return EXIT_FAILURE;
    }

    start_game(grid_size, num_layers, num_steps, density, seed);

    return EXIT_SUCCESS;
}