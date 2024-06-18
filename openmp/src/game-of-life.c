#include "game-of-life.h"

#include <stdlib.h>

void init_gol(gol_t* gol, const uint32_t grid_size, const uint32_t seed, const float density) {
    srand(seed);

    // size of the grid + 2 for the ghost cells
    gol->size = grid_size + 2;
    size_t size = (gol->size) * (gol->size) * sizeof(cell_t);

    gol->current = (cell_t*) malloc(size);
    gol->next = (cell_t*) malloc(size);
}