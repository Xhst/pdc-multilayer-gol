#include "ml_gol.h"

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

void start_game(const uint64_t grid_size, const uint64_t num_layers, const uint64_t num_steps, const bool create_png, const float density, const uint64_t seed) {
    ml_gol_t* ml_gol = (ml_gol_t*) malloc(sizeof(ml_gol_t));

    init_ml_gol(ml_gol, grid_size, num_layers, create_png, density, seed);

    printf("Starting simulation with %ld steps and %d threads\n", num_steps, omp_get_num_threads());

    for (uint64_t s = 1; s < num_steps; s++) {
#pragma omp parallel for
        for (uint64_t layer = 0; layer < num_layers; layer++) {
            step(&ml_gol->layers[layer]);
        }
       
        calculate_combined(ml_gol); 
        calculate_dependent(ml_gol);

        if (create_png) {
            create_png_for_step(ml_gol, s);
        }

        reset_combined_and_dependent(ml_gol);
    }
    
    free_ml_gol(ml_gol);
}


void calculate_combined(const ml_gol_t* ml_gol) {
    for (uint64_t layer = 0; layer < ml_gol->num_layers; layer++) {
#pragma omp parallel for collapse(2)
        for (uint64_t i = 1; i < ml_gol->grid_size + 1; i++) {
            for (uint64_t j = 1; j < ml_gol->grid_size + 1; j++) {
                size_t grid_idx = idx(&ml_gol->layers[layer], i, j);
                size_t combined_idx = (i - 1) * ml_gol->grid_size + (j - 1);
                
                // If the cell is alive in the current layer, add the color of the layer to the combined grid
                if (ml_gol->layers[layer].current[grid_idx]) {
                    ml_gol->combined[combined_idx] = add_colors(ml_gol->combined[combined_idx], ml_gol->layers_colors[layer]);
                }
            }
        }
    }
}


void create_png_for_grid(const color_t* grid, const uint64_t grid_size, const uint64_t step, const char* folder) {
    char filename[50];

    // 3 channels: RGB
    const uint8_t channels = 3;
    uint8_t* buffer = (uint8_t*) malloc(grid_size * grid_size * channels * sizeof(uint8_t));

#pragma omp parallel for collapse(2)
    for (uint64_t i = 0; i < grid_size; i++) {
        for (uint64_t j = 0; j < grid_size; j++) {
            uint64_t idx = (i * grid_size + j);

            buffer[idx * channels] =     grid[idx].r;
            buffer[idx * channels + 1] = grid[idx].g;
            buffer[idx * channels + 2] = grid[idx].b;
        }
    }

    sprintf(filename, "output/%s/%s%04ld.png", folder, folder, step);
    write_png_file(filename, grid_size, grid_size, buffer);

    free(buffer);
}

void create_png_for_step(const ml_gol_t* ml_gol, const uint64_t step) {
    create_png_for_grid(ml_gol->combined, ml_gol->grid_size, step, "combined");
    create_png_for_grid(ml_gol->dependent, ml_gol->grid_size, step, "dependent");
}

void reset_combined_and_dependent(ml_gol_t* ml_gol) {
#pragma omp parallel for
    for (uint64_t i = 0; i < ml_gol->grid_size * ml_gol->grid_size; i++) {
        ml_gol->combined[i] = BLACK;
        ml_gol->dependent[i] = BLACK;
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

void init_ml_gol(ml_gol_t* ml_gol, const uint64_t grid_size, const uint64_t num_layers, const bool create_png, const float density, const uint64_t seed) {
    srand(seed);

    ml_gol->num_layers = num_layers;
    ml_gol->layers = (gol_t*) malloc(num_layers * sizeof(gol_t));
    ml_gol->layers_colors = (color_t*) malloc(num_layers * sizeof(color_t));

    ml_gol->grid_size = grid_size;

    for (uint64_t i = 0; i < ml_gol->num_layers; i++) {
        init_gol(&ml_gol->layers[i], grid_size, density);
        ml_gol->layers_colors[i] = get_color_for_layer(i, num_layers);
    }

    size_t size = (ml_gol->grid_size) * (ml_gol->grid_size) * sizeof(color_t);

    ml_gol->combined = (color_t*) malloc(size * sizeof(color_t));
    ml_gol->dependent = (color_t*) malloc(size * sizeof(color_t));

    calculate_combined(ml_gol); 
    calculate_dependent(ml_gol);

    if (create_png) {
        create_png_for_step(ml_gol, 0);
    }

    reset_combined_and_dependent(ml_gol);

    printf("Initialized multilayer game of life with %ld layers and grid size %ld\n", num_layers, grid_size);
    
    print_layers_colors(ml_gol);
}

color_t get_color_for_layer(const uint64_t layer, const uint64_t num_layers) {
    // Calculate the angle for the hue based on the layer
    double hue = (double) layer / num_layers * 360.0;

    color_hsv_t hsv_color;
    hsv_color.h = hue;
    hsv_color.s = 1.0; 
    hsv_color.v = 1.0;

    return hsv_to_rgb(hsv_color);
}

uint8_t count_dependent_alive_neighbors(const ml_gol_t* ml_gol, const uint64_t i, const uint64_t j) {
    uint8_t count = 0;

    for (int32_t x = -1; x <= 1; x++) {
        for (int32_t y = -1; y <= 1; y++) {
            uint64_t layer_idx = idx(&ml_gol->layers[0], i + x, j + y);

            for (uint64_t layer = 0; layer < ml_gol->num_layers; layer++) {
                if (ml_gol->layers[layer].current[layer_idx]) {
                    count++;
                }
            }
        }
    }

    return count;
}

void calculate_dependent(const ml_gol_t* ml_gol) {
#pragma omp parallel for collapse(2)
    for (uint64_t i = 1; i < ml_gol->grid_size - 1; i++) {
        for (uint64_t j = 1; j < ml_gol->grid_size - 1; j++) {
            uint64_t idx = i * ml_gol->grid_size + j;
            
            uint8_t alive_neighbors = count_dependent_alive_neighbors(ml_gol, i, j);

            uint8_t channel_value = (uint8_t) ((((float) alive_neighbors) / (9 * ml_gol->num_layers)) * 255);
            
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