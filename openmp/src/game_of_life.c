#include "game_of_life.h"


void init_gol(gol_t* gol, const uint32_t grid_size, const float density) {
    // size of the grid + 2 for the ghost cells
    gol->size = grid_size + 2;
    size_t size = (gol->size) * (gol->size) * sizeof(bool);

    gol->current = (bool*) malloc(size);
    gol->next = (bool*) malloc(size);

    init_grid(gol, density);
}

void init_grid(const gol_t* gol, const float density) {
    for (uint32_t i = 1; i < gol->size - 1; i++) {
        for (uint32_t j = 1; j < gol->size - 1; j++) {
            gol->current[idx(gol, i, j)] = (((float) rand()) / RAND_MAX) < density;
        }
    }
}

uint8_t count_alive_neighbors(const gol_t* gol, const uint32_t i, const uint32_t j) {
    return  gol->current[idx(gol, i - 1, j - 1)] + gol->current[idx(gol, i - 1, j)] + gol->current[idx(gol, i - 1, j + 1)] +
            gol->current[idx(gol, i, j - 1)    ]                                    + gol->current[idx(gol, i, j + 1)    ] +
            gol->current[idx(gol, i + 1, j - 1)] + gol->current[idx(gol, i + 1, j)] + gol->current[idx(gol, i + 1, j + 1)];
}

void swap_grids(gol_t* gol) {
    bool* temp = gol->current;
    gol->current = gol->next;
    gol->next = temp;
}

void step(const gol_t* gol) {
#pragma omp parallel for collapse(2)
    for (uint32_t i = 1; i < gol->size - 1; i++) {
        for (uint32_t j = 1; j < gol->size - 1; j++) {

            uint8_t alive_neighbors = count_alive_neighbors(gol, i, j);

            // The state of the current cell
            bool is_alive = gol->current[idx(gol, i, j)];

            // The state of the current cell in the next step based on the rules of the game of life
            bool next_state = (is_alive && !(alive_neighbors < 2 || alive_neighbors > 3)) || (!is_alive && alive_neighbors == 3);

            gol->next[idx(gol, i, j)] = next_state;
        }
    }
}

void fill_ghost_cells(const gol_t* gol) {
    // Left and right borders
#pragma omp parallel for
    for (uint32_t i = 1; i < gol->size - 1; i++) {
        gol->current[idx(gol, i, 0)] = gol->current[idx(gol, i, gol->size - 2)];
        gol->current[idx(gol, i, gol->size - 1)] = gol->current[idx(gol, i, 1)];
    }

    // Top and bottom borders
#pragma omp parallel for
    for (uint32_t j = 0; j < gol->size; j++) {
        gol->current[idx(gol, 0, j)] = gol->current[idx(gol, gol->size - 2, j)];
        gol->current[idx(gol, gol->size - 1, j)] = gol->current[idx(gol, 1, j)];
    }
    
    // Corners
    gol->current[idx(gol, 0, 0)] = gol->current[idx(gol, gol->size - 2, gol->size - 2)];
    gol->current[idx(gol, 0, gol->size - 1)] = gol->current[idx(gol, gol->size - 2, 1)];
    gol->current[idx(gol, gol->size - 1, 0)] = gol->current[idx(gol, 1, gol->size - 2)];
    gol->current[idx(gol, gol->size - 1, gol->size - 1)] = gol->current[idx(gol, 1, 1)];
}

void free_gol(gol_t* gol) {
    free(gol->current);
    free(gol->next);
}
