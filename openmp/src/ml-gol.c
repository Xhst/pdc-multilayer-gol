#include "ml-gol.h"

#include <stdlib.h>
#include <stdio.h>

void start_game(uint32_t grid_size, uint32_t num_layers, uint32_t num_steps, uint32_t seed) {
    srand(seed);

    printf("Grid size: %u\n", grid_size);
    printf("Number of layers: %u\n", num_layers);
    printf("Number of steps: %u\n", num_steps);
    printf("Seed: %u\n", seed);
}
