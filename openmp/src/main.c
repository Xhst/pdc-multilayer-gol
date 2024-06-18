#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "converter.h"
#include "ml-gol.h"

#define DEFAULT_GRID_SIZE 128
#define DEFAULT_NUM_LAYERS 3
#define DEFAULT_NUM_STEPS 64

int main(int argc, char *argv[]) {
    
    uint32_t grid_size = DEFAULT_GRID_SIZE;
    uint32_t num_layers = DEFAULT_NUM_LAYERS;
    uint32_t num_steps = DEFAULT_NUM_STEPS;
    uint32_t seed = time(NULL);

    if (argc > 1) {
        grid_size = atouint32(argv[1]);
    }

    if (argc > 2) {
        num_layers = atouint32(argv[2]);
    }

    if (argc > 3) {
        num_steps = atouint32(argv[3]);
    }

    if (argc > 4) {
        seed = atouint32(argv[4]);
    }

    if (grid_size == 0 || num_layers == 0 || num_steps == 0) {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }

    start_game(grid_size, num_layers, num_steps, seed);

    return 0;
}