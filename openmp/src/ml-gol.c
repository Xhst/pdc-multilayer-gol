#include "ml-gol.h"

#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_DENSITY 0.3

void start_game(const uint32_t grid_size, const uint32_t num_layers, const uint32_t num_steps, const uint32_t seed) {
    srand(seed);

    ml_gol_t* ml_gol = (ml_gol_t*) malloc(sizeof(ml_gol_t));
    init_ml_gol(ml_gol, grid_size, num_layers, seed);

    for (uint32_t s = 0; s < num_steps; s++) {
        for (uint32_t i = 0; i < num_layers; i++) {
            fill_ghost_cells(&ml_gol->layers[i]);
            step(&ml_gol->layers[i]);
            swap_grids(&ml_gol->layers[i]);
        }
        calculate_combined(ml_gol);
    }

    free_ml_gol(ml_gol);
}

void init_ml_gol(ml_gol_t* ml_gol, const uint32_t grid_size, const uint32_t num_layers, const uint32_t seed) {
    ml_gol->num_layers = num_layers;
    ml_gol->layers = (gol_t*) malloc(num_layers * sizeof(gol_t));

    // size of the grid + 2 for the ghost cells
    ml_gol->grid_size = grid_size + 2;

    for (uint32_t i = 0; i < ml_gol->num_layers; i++) {
        init_gol(&ml_gol->layers[i], ml_gol->grid_size, seed * (i + 1) % ml_gol->num_layers, DEFAULT_DENSITY);
    }

    size_t size = (ml_gol->grid_size) * (ml_gol->grid_size) * sizeof(color_t);

    ml_gol->combined = (color_t*) malloc(size * sizeof(color_t));
    ml_gol->dependent = (color_t*) malloc(size * sizeof(color_t));
}

void calculate_combined(const ml_gol_t* ml_gol) {
    for (uint32_t i = 0; i < ml_gol->grid_size; i++) {
        for (uint32_t j = 0; j < ml_gol->grid_size; j++) {
            uint32_t idx = i * ml_gol->grid_size + j;

            ml_gol->combined[idx] = 0;
            for (uint32_t k = 0; k < ml_gol->num_layers; k++) {
                 ml_gol->combined[idx] += ml_gol->layers[k].current[idx];
            }
        }
    }
}

void free_ml_gol(ml_gol_t* ml_gol) {
    for (uint32_t i = 0; i < ml_gol->num_layers; i++) {
        free_gol(&ml_gol->layers[i]);
    }

    free(ml_gol->layers);
    free(ml_gol->combined);
    free(ml_gol->dependent);
    free(ml_gol);
}