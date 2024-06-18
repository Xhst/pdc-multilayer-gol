#include "game-of-life.h"

#include <stdlib.h>

#define IDX(i, j) ((i) * (gol->size) + (j))

void init_gol(gol_t* gol, const uint32_t grid_size, const uint32_t seed, const float density) {
    srand(seed);

    // size of the grid + 2 for the ghost cells
    gol->size = grid_size + 2;
    size_t size = (gol->size) * (gol->size) * sizeof(cell_t);

    gol->current = (cell_t*) malloc(size);
    gol->next = (cell_t*) malloc(size);

    init_grid(gol, density);
}

void init_grid(const gol_t* gol, const float density) {
    for (uint32_t i = 1; i < gol->size - 1; i++) {
        for (uint32_t j = 1; j < gol->size - 1; j++) {
            gol->current[IDX(i, j)] = (((float) rand()) / RAND_MAX < density);
        }
    }
}