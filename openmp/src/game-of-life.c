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

uint8_t count_alive_neighbors(const gol_t* gol, const uint32_t i, const uint32_t j) {
    return  gol->current[IDX(i - 1, j - 1)] + gol->current[IDX(i - 1, j)] + gol->current[IDX(i - 1, j + 1)] +
            gol->current[IDX(i, j - 1)    ]                               + gol->current[IDX(i, j + 1)    ] +
            gol->current[IDX(i + 1, j - 1)] + gol->current[IDX(i + 1, j)] + gol->current[IDX(i + 1, j + 1)];
}

void swap_grids(gol_t* gol) {
    cell_t* temp = gol->current;
    gol->current = gol->next;
    gol->next = temp;
}

void step(const gol_t* gol) {
    for (uint32_t i = 1; i < gol->size - 1; i++) {
        for (uint32_t j = 1; j < gol->size - 1; j++) {

            uint8_t alive_neighbors = count_alive_neighbors(gol, i, j);

            // The state of the current cell
            cell_t is_alive = gol->current[IDX(i, j)];

            // The state of the current cell in the next step based on the rules of the game of life
            cell_t next_state = (is_alive && !(alive_neighbors < 2 || alive_neighbors > 3)) || (!is_alive && alive_neighbors == 3);

            gol->next[IDX(i, j)] = next_state;
        }
    }
}

void fill_ghost_cells(const gol_t* gol) {
    // Left and right borders
    for (uint32_t i = 1; i < gol->size - 1; i++) {
        gol->current[IDX(i, 0)] = gol->current[IDX(i, gol->size - 2)];
        gol->current[IDX(i, gol->size - 1)] = gol->current[IDX(i, 1)];
    }

    // Top and bottom borders
    for (uint32_t j = 0; j < gol->size; j++) {
        gol->current[IDX(0, j)] = gol->current[IDX(gol->size - 2, j)];
        gol->current[IDX(gol->size - 1, j)] = gol->current[IDX(1, j)];
    }
    
    // Corners
    gol->current[IDX(0, 0)] = gol->current[IDX(gol->size - 2, gol->size - 2)];
    gol->current[IDX(0, gol->size - 1)] = gol->current[IDX(gol->size - 2, 1)];
    gol->current[IDX(gol->size - 1, 0)] = gol->current[IDX(1, gol->size - 2)];
    gol->current[IDX(gol->size - 1, gol->size - 1)] = gol->current[IDX(1, 1)];
}

void free_gol(gol_t* gol) {
    free(gol->current);
    free(gol->next);
    free(gol);
}