#include "ml-gol.h"

#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_DENSITY 0.3

void init_layers(ml_gol_t* ml_gol, const uint32_t grid_size, const uint32_t seed) {
    for (uint32_t i = 0; i < ml_gol->num_layers; i++) {
        init_gol(&ml_gol->layers[i], grid_size, seed * (i + 1) % ml_gol->num_layers, DEFAULT_DENSITY);
    }
}

void start_game(const uint32_t grid_size, const uint32_t num_layers, const uint32_t num_steps, const uint32_t seed) {
    srand(seed);

    ml_gol_t* ml_gol = (ml_gol_t*) malloc(sizeof(ml_gol_t));
    ml_gol->num_layers = num_layers;
    ml_gol->layers = (gol_t*) malloc(num_layers * sizeof(gol_t));
    ml_gol->grid_size = grid_size;

    init_layers(ml_gol, grid_size, seed);

    ml_gol->combined = (cell_t*) malloc(grid_size * grid_size * sizeof(cell_t));
    ml_gol->dependent = (cell_t*) malloc(grid_size * grid_size * sizeof(cell_t));

    for (uint32_t s = 0; s < num_steps; s++) {
        for (uint32_t i = 0; i < num_layers; i++) {
            fill_ghost_cells(&ml_gol->layers[i]);
            step(&ml_gol->layers[i]);
            swap_grids(&ml_gol->layers[i]);
            calculate_dependent(ml_gol);
        }
    }

    free_ml_gol(ml_gol);
}

void calculate_dependent(const ml_gol_t* ml_gol) {
    for (uint32_t i = 0; i < ml_gol->grid_size; i++) {
        for (uint32_t j = 0; j < ml_gol->grid_size; j++) {
            uint32_t dep_idx = i * ml_gol->grid_size + j;
            uint32_t lay_idx = (i + 1) * (ml_gol->grid_size + 2) + (j + 1);

            ml_gol->dependent[dep_idx] = 0;
            for (uint32_t l = 1; l < ml_gol->num_layers; l++) {
                ml_gol->dependent[dep_idx] += ml_gol->layers[l].current[lay_idx];
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
