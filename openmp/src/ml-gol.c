#include "ml-gol.h"

#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_DENSITY 0.3

void start_game(const uint32_t grid_size, const uint32_t num_layers, const uint32_t num_steps, const uint32_t seed) {
    srand(seed);

    ml_gol_t* ml_gol = (ml_gol_t*) malloc(sizeof(ml_gol_t));
    ml_gol->num_layers = num_layers;
    ml_gol->layers = (gol_t*) malloc(num_layers * sizeof(gol_t));

    for (uint32_t i = 0; i < num_layers; i++) {
        init_gol(&ml_gol->layers[i], grid_size, seed * (i + 1) % num_layers, DEFAULT_DENSITY);
    }

}
