#include "game_of_life.h"


void init_gol(gol_t* gol, const uint64_t grid_size, const float density) {
    gol->size = grid_size;

    // size of the grid + 2 for the ghost cells
    size_t size = (gol->size + 2) * (gol->size + 2) * sizeof(bool);

    gol->current = (bool*) malloc(size);
    gol->next = (bool*) malloc(size);

    init_grid(gol, density);
}

void init_grid(const gol_t* gol, const float density) {
    for (uint64_t i = 1; i < gol->size + 1; i++) {
        for (uint64_t j = 1; j < gol->size + 1; j++) {
            gol->current[idx(gol, i, j)] = ((float) rand() / RAND_MAX) < density;
        }
    }
}

uint8_t count_alive_neighbors(const gol_t* gol, const uint64_t i, const uint64_t j) {
    return  gol->current[idx(gol, i - 1, j - 1)] + gol->current[idx(gol, i - 1, j)] + gol->current[idx(gol, i - 1, j + 1)] +
            gol->current[idx(gol, i, j - 1)    ]                                    + gol->current[idx(gol, i, j + 1)    ] +
            gol->current[idx(gol, i + 1, j - 1)] + gol->current[idx(gol, i + 1, j)] + gol->current[idx(gol, i + 1, j + 1)];
}

void swap_grids(gol_t* gol) {
    bool* temp = gol->current;
    gol->current = gol->next;
    gol->next = temp;
}

void step(gol_t* gol) {
    fill_ghost_cells(gol);

    for (uint64_t i = 1; i < gol->size + 1; i++) {
        for (uint64_t j = 1; j < gol->size + 1; j++) {

            uint8_t alive_neighbors = count_alive_neighbors(gol, i, j);

            // The state of the current cell
            bool is_alive = gol->current[idx(gol, i, j)];

            // The state of the current cell in the next step based on the rules of the game of life
            bool next_state = (is_alive && !(alive_neighbors < 2 || alive_neighbors > 3)) || (!is_alive && alive_neighbors == 3);

            gol->next[idx(gol, i, j)] = next_state;
        }
    }

    swap_grids(gol);
}

void fill_ghost_cells(const gol_t* gol) {
    const uint64_t TOP = 1;
    const uint64_t BOTTOM = gol->size - 1;
    const uint64_t LEFT = 1;
    const uint64_t RIGHT = gol->size - 1;
    const uint64_t HALO_TOP = TOP - 1;
    const uint64_t HALO_BOTTOM = BOTTOM + 1;
    const uint64_t HALO_LEFT = LEFT - 1;
    const uint64_t HALO_RIGHT = RIGHT + 1;

    // Left and right borders
    for (uint64_t i = TOP; i < BOTTOM + 1; i++) {
        gol->current[idx(gol, i, HALO_LEFT)]  = gol->current[idx(gol, i, RIGHT)];
        gol->current[idx(gol, i, HALO_RIGHT)] = gol->current[idx(gol, i, LEFT)];
    }

    // Top and bottom borders
    for (uint64_t j = LEFT; j < RIGHT + 1; j++) {
        gol->current[idx(gol, HALO_TOP, j)]    = gol->current[idx(gol, BOTTOM, j)];
        gol->current[idx(gol, HALO_BOTTOM, j)] = gol->current[idx(gol, TOP, j)];
    }
    
    // Corners
    gol->current[idx(gol, HALO_TOP, HALO_LEFT)]     = gol->current[idx(gol, BOTTOM, RIGHT)];
    gol->current[idx(gol, HALO_TOP, HALO_RIGHT)]    = gol->current[idx(gol, BOTTOM, LEFT)];
    gol->current[idx(gol, HALO_BOTTOM, HALO_LEFT)]  = gol->current[idx(gol, TOP, RIGHT)];
    gol->current[idx(gol, HALO_BOTTOM, HALO_RIGHT)] = gol->current[idx(gol, TOP, LEFT)];
}

void free_gol(gol_t* gol) {
    free(gol->current);
    free(gol->next);
}
