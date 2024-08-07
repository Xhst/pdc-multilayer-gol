/**
 * @file main.c
 * @brief Main file for the OpenMP implementation of the Game of Life
 * 
 * How to compile:
 * move to the openmp directory and run 'make' command.
 * Check the Makefile for more details.
 * 
 * How to run (from the openmp directory):
 * ./bin/multilayer-game-of-life <grid_size> <num_layers> <density> <num_steps> <seed>
 * The parameters are optional, if not provided, the default values are used.
 */

extern "C" {

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "converter.h"

}

#include "ml_gol.cuh"

#define DEFAULT_GRID_SIZE 128
#define DEFAULT_NUM_LAYERS 3
#define DEFAULT_NUM_STEPS 64
#define DEFAULT_CREATE_PNG true
#define DEFAULT_DENSITY 0.3
#define DEFAULT_USE_SHARED false

int main(int argc, char *argv[]) {
    
    uint64_t grid_size = DEFAULT_GRID_SIZE;
    uint64_t num_layers = DEFAULT_NUM_LAYERS;
    uint64_t num_steps = DEFAULT_NUM_STEPS;
    bool create_png = DEFAULT_CREATE_PNG;
    float density = DEFAULT_DENSITY;
    bool use_shared = DEFAULT_USE_SHARED;
    uint64_t seed = time(NULL);

    if (argc > 1) {
        grid_size = atouint64(argv[1]);
    }

    if (argc > 2) {
        num_layers = atouint64(argv[2]);
    }

    if (argc > 3) {
        num_steps = atouint64(argv[3]);
    }

    if (argc > 4) {
        create_png = atoi(argv[4]) != 0;
    }

    if (argc > 5) {
        use_shared = atoi(argv[5]) != 0;
    }

    if (argc > 6) {
        seed = atouint64(argv[6]);
    }

    if (argc > 7) {
        density = atof(argv[7]);
    }

    if (grid_size == 0 || num_layers == 0 || num_steps == 0) {
        fprintf(stderr, "Invalid input\n");
        return EXIT_FAILURE;
    }

    
    printf("Starting Multilayer Game of Life on CUDA\n");

    start_game_on_cuda(grid_size, num_layers, num_steps, create_png, density, seed, use_shared);

    return EXIT_SUCCESS;
}