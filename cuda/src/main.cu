/**
 * @file main.c
 * @brief Main file for the OpenMP implementation of the Game of Life
 * 
 * How to compile:
 * move to the openmp directory and run 'make' command.
 * Check the Makefile for more details.
 * 
 * How to run (from the openmp directory):
 * ./bin/multilayer-game-of-life <grid_size> <num_layers> <density> <num_steps> <seed>
 * The parameters are optional, if not provided, the default values are used.
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>


#include "vector_add.cuh"


#include "converter.h"
#include "ml_gol.h"

#define DEFAULT_GRID_SIZE 128
#define DEFAULT_NUM_LAYERS 3
#define DEFAULT_NUM_STEPS 64
#define DEFAULT_CREATE_PNG true
#define DEFAULT_DENSITY 0.3

int main(int argc, char *argv[]) {
    
    uint64_t grid_size = DEFAULT_GRID_SIZE;
    uint64_t num_layers = DEFAULT_NUM_LAYERS;
    uint64_t num_steps = DEFAULT_NUM_STEPS;
    bool create_png = DEFAULT_CREATE_PNG;
    float density = DEFAULT_DENSITY;
    uint64_t seed = time(NULL);

    if (argc > 1) {
        grid_size = atouint64(argv[1]);
    }

    if (argc > 2) {
        num_layers = atouint64(argv[2]);
    }

    if (argc > 3) {
        num_steps = atouint64(argv[3]);
    }

    if (argc > 4) {
        create_png = atoi(argv[4]) != 0;
    }

    if (argc > 5) {
        density = atof(argv[4]);
    }

    if (argc > 6) {
        seed = atouint64(argv[4]);
    }

    if (grid_size == 0 || num_layers == 0 || num_steps == 0) {
        fprintf(stderr, "Invalid input\n");
        return EXIT_FAILURE;
    }


    /*####################################################################
    ########################    CUDA TEST   ##############################
    ######################################################################*/

    float *h_a, *h_b, *h_c; // host copies of a, b, c
    float *d_a, *d_b, *d_c; // device copies of a, b, c
    int size = N * sizeof(float);

    // Allocate space for device copies of a, b, c
    cudaMalloc((void **)&d_a, size);
    cudaMalloc((void **)&d_b, size);
    cudaMalloc((void **)&d_c, size);

    // Allocate space for host copies of a, b, c and setup input values
    h_a = (float *)malloc(size);
    h_b = (float *)malloc(size);
    h_c = (float *)malloc(size);
    for (int i = 0; i < N; i++) {
        h_a[i] = i;
        h_b[i] = i * 2;
    }

    // Copy inputs to device
    cudaMemcpy(d_a, h_a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, h_b, size, cudaMemcpyHostToDevice);

    // Launch vectorAdd() kernel on GPU with N blocks
    vectorAdd<<<(N + 255) / 256, 256>>>(d_a, d_b, d_c, N);

    // Copy result back to host
    cudaMemcpy(h_c, d_c, size, cudaMemcpyDeviceToHost);

    // Print the result
    for (int i = 0; i < N; i++) {
        if (i % 100 == 0) {
            printf("h_c[%d] = %f\n", i, h_c[i]);
        }
    }

    // Cleanup
    free(h_a);
    free(h_b);
    free(h_c);
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);

    /* 
    printf("Starting Multilayer Game of Life.\n");
    printf("Max num of threads: %d\n",  omp_get_max_threads());

    double tstart, tstop;
    tstart = omp_get_wtime();

    start_game(grid_size, num_layers, num_steps, create_png, density, seed);

    tstop = omp_get_wtime();
    printf("Elapsed time: %f\n", tstop - tstart); 
    */

    return EXIT_SUCCESS;
}