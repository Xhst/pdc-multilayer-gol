#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "ml-gol.h"

#define DEFAULT_GRID_SIZE 128
#define DEFAULT_NUM_STEPS 64

int main(int argc, char *argv[]) {
    
    uint32_t grid_size = DEFAULT_GRID_SIZE;
    uint32_t num_steps = DEFAULT_NUM_STEPS;
    uint32_t seed = time(NULL);

    if (argc > 1) {
        grid_size = atoi(argv[1]);
    }

    if (argc > 2) {
        num_steps = atoi(argv[2]);
    }

    if (argc > 3) {
        seed = atoi(argv[3]);
    }

    start_game(grid_size, num_steps, seed);

    return 0;
}