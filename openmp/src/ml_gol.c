#include "ml_gol.h"

#include <stdlib.h>
#include <stdio.h>


void start_game(const uint64_t grid_size, const uint64_t num_layers, const uint64_t num_steps, const bool create_png, const float density, const uint64_t seed) {
    ml_gol_t* ml_gol = (ml_gol_t*) malloc(sizeof(ml_gol_t));

    init_ml_gol(ml_gol, grid_size, num_layers, density, seed);
#pragma omp parallel
    {
        for (uint64_t s = 0; s < num_steps; s++) {
#pragma omp for
            for (uint64_t i = 0; i < num_layers; i++) {
                fill_ghost_cells(&ml_gol->layers[i]);
                step(&ml_gol->layers[i]);
                swap_grids(&ml_gol->layers[i]);
            }
            
            calculate_combined(ml_gol);
            calculate_dependent(ml_gol);

            if (create_png) create_png_for_step(ml_gol, s);

            reset_combined_and_dependent(ml_gol);
        }
    }
    
    free_ml_gol(ml_gol);
}

void create_png_for_grid(const color_t* grid, const uint64_t grid_size, const uint64_t step, const char* folder) {
    char filename[50];

    // 3 channels: RGB
    const uint8_t channels = 3;
    uint8_t buffer[grid_size * grid_size * channels];

#pragma omp for collapse(2)
    for (uint64_t i = 0; i < grid_size; i++) {
        for (uint64_t j = 0; j < grid_size; j++) {
            uint64_t idx = (i * grid_size + j) * channels;
            uint64_t grid_idx = i * grid_size + j;

            buffer[idx] = grid[grid_idx].r;
            buffer[idx + 1] = grid[grid_idx].g;
            buffer[idx + 2] = grid[grid_idx].b;
        }
    }

    sprintf(filename, "output/%s/%s%04ld.png", folder, folder, step);
    write_png_file(filename, grid_size, grid_size, buffer);
}

void create_png_for_step(const ml_gol_t* ml_gol, const uint64_t step) {
    // The grid size without the ghost cells
    uint64_t grid_size = ml_gol->grid_size - 2;

    create_png_for_grid(ml_gol->combined, grid_size, step, "combined");
    create_png_for_grid(ml_gol->dependent, grid_size, step, "dependent");
}

void reset_combined_and_dependent(ml_gol_t* ml_gol) {
#pragma omp for collapse(2) 
    for (uint64_t i = 0; i < ml_gol->grid_size; i++) {
        for (uint64_t j = 0; j < ml_gol->grid_size; j++) {
            uint64_t idx = i * ml_gol->grid_size + j;
            ml_gol->combined[idx] = BLACK;
            ml_gol->dependent[idx] = BLACK;
        }
    }
}

void print_layers_colors(const ml_gol_t* ml_gol) {
    char hex[8];
    printf("Colors for the layers:\n");
    for (uint64_t i = 0; i < ml_gol->num_layers; i++) {
        color_to_hex(ml_gol->layers_colors[i], hex);
        printf("Layer %ld: %s\n", i, hex);
    }
}

void init_ml_gol(ml_gol_t* ml_gol, const uint64_t grid_size, const uint64_t num_layers, const float density, const uint64_t seed) {
    srand(seed);

    ml_gol->num_layers = num_layers;
    ml_gol->layers = (gol_t*) malloc(num_layers * sizeof(gol_t));
    ml_gol->layers_colors = (color_t*) malloc(num_layers * sizeof(color_t));

    // size of the grid + 2 for the ghost cells
    ml_gol->grid_size = grid_size + 2;

    for (uint64_t i = 0; i < ml_gol->num_layers; i++) {
        init_gol(&ml_gol->layers[i], ml_gol->grid_size, density);
        ml_gol->layers_colors[i] = get_color_for_layer(i, num_layers);
    }

    size_t size = (ml_gol->grid_size) * (ml_gol->grid_size) * sizeof(color_t);

    ml_gol->combined = (color_t*) malloc(size * sizeof(color_t));
    ml_gol->dependent = (color_t*) malloc(size * sizeof(color_t));

    reset_combined_and_dependent(ml_gol);

    printf("Initialized multilayer game of life with %ld layers and grid size %ld\n", num_layers, grid_size);
    
    print_layers_colors(ml_gol);
}

color_t get_color_for_layer(const uint64_t layer, const uint64_t num_layers) {
    // Calculate the angle for the hue based on the layer
    double hue = (360.0 * layer) / num_layers; 

    color_hsv_t hsv_color;
    hsv_color.h = hue;
    hsv_color.s = 1.0; 
    hsv_color.v = 1.0;

    return hsv_to_rgb(hsv_color);
}

void calculate_combined(const ml_gol_t* ml_gol) {
#pragma omp for collapse(3)
    for (uint64_t k = 0; k < ml_gol->num_layers; k++) {         
        for (uint64_t i = 0; i < ml_gol->grid_size; i++) {
            for (uint64_t j = 0; j < ml_gol->grid_size; j++) {
                uint64_t idx = i * ml_gol->grid_size + j;

                if (!ml_gol->layers[k].current[idx]) continue;

                ml_gol->combined[idx] = add_colors(ml_gol->combined[idx], ml_gol->layers_colors[k]);
            }
        }
    }
}

uint8_t count_dependent_alive_neighbors(const ml_gol_t* ml_gol, const uint64_t i, const uint64_t j) {
    uint8_t count = 0;

    for (int32_t x = -1; x <= 1; x++) {
        for (int32_t y = -1; y <= 1; y++) {
            if (x == 0 && y == 0) continue;

            uint64_t idx = (i + x) * ml_gol->grid_size + (j + y);

            if (ml_gol->combined[idx].r > 0 && ml_gol->combined[idx].g > 0 && ml_gol->combined[idx].b > 0) {
                count++;
            }
        }
    }

    return count;
}

void calculate_dependent(const ml_gol_t* ml_gol) {
#pragma omp for collapse(2)
    for (uint64_t i = 1; i < ml_gol->grid_size - 1; i++) {
        for (uint64_t j = 1; j < ml_gol->grid_size - 1; j++) {
            uint64_t idx = i * ml_gol->grid_size + j;
            
            uint8_t alive_neighbors = count_dependent_alive_neighbors(ml_gol, i, j);

            uint8_t channel_value = (uint8_t) ((((float) alive_neighbors) / 8) * 255);
            
            ml_gol->dependent[idx] = (color_t){channel_value, channel_value, channel_value};
            
        }
    }
}

void free_ml_gol(ml_gol_t* ml_gol) {
    for (uint64_t i = 0; i < ml_gol->num_layers; i++) {
        free_gol(&ml_gol->layers[i]);
    }

    free(ml_gol->layers);
    free(ml_gol->combined);
    free(ml_gol->dependent);
    free(ml_gol);
}