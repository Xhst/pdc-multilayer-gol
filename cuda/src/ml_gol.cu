extern "C" {
#include "hpc.h"
#include "image.h"
#include "color.h"

#include <stdlib.h>
#include <stdio.h>

}

#include "ml_gol.cuh"

#define BLKDIM 32

#define cudaCheckErrors(msg) \
    do { \
        cudaError_t __err = cudaGetLastError(); \
        if (__err != cudaSuccess) { \
            fprintf(stderr, "Fatal error: %s (%s at %s:%d)\n", \
                msg, cudaGetErrorString(__err), \
                __FILE__, __LINE__); \
            fprintf(stderr, "*** FAILED - ABORTING\n"); \
            exit(1); \
        } \
    } while (0)


void start_game_on_cuda(const uint64_t grid_size, const uint64_t num_layers, const uint64_t num_steps, const bool create_png, const float density, const uint64_t seed) {
    ml_gol_t* ml_gol;
    ml_gol = (ml_gol_t*) malloc(sizeof(ml_gol_t));
    init_ml_gol(ml_gol, grid_size, num_layers, density, seed);

    /**
     * Uncomment to use managed unified memory.
     * Automatically manages memory transfers between GPU and CPU.
     * Simpler implementation but slower, especially with our data structures.
    */
    /* cudaMallocManaged(&ml_gol, sizeof(ml_gol_t));
    init_ml_gol_managed(ml_gol, grid_size, num_layers, density, seed); */
    
    bool* d_current = nullptr;
    bool* d_next = nullptr;
    color_t* d_layers_colors = nullptr;
    color_t* d_combined = nullptr;
    color_t* d_dependent = nullptr;
    init_ml_gol_to_device(ml_gol, &d_current, &d_next, &d_layers_colors, &d_combined, &d_dependent); 
    
    // we create blocks of 1024 threads, one thread per cell (considering all layers)
    dim3 gridDim((grid_size + BLKDIM - 1) / BLKDIM, (grid_size + BLKDIM - 1) / BLKDIM);
    dim3 blockDim(BLKDIM, BLKDIM);

    if (create_png) {
        create_all_pngs(ml_gol, &d_current, &d_next, &d_layers_colors, &d_combined, &d_dependent, num_steps, gridDim, blockDim);
    } else {
        /* Insert this as third argument to use shared memory (remember changes in kernel)
        (BLKDIM + 2) * (BLKDIM + 2) * num_layers
        */
        fill_ghost_cells(ml_gol->current, ml_gol->grid_size, ml_gol->num_layers);
        update_d_ml_gol(ml_gol, &d_current, &d_next, &d_combined, &d_dependent);

        /* cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, 0);
        printf("Max threads per block: %d VS threads launched per block: %d\n", prop.maxThreadsPerBlock, blockDim.x * blockDim.y * blockDim.z);
        printf("Max blocks in each dimension: %d, %d, %d VS blocks launched: %d, %d. %d\n", prop.maxGridSize[0], prop.maxGridSize[1], prop.maxGridSize[2], gridDim.x, gridDim.y, gridDim.z);
        printf("Max registers per block: %d VS register used IDK\n", prop.regsPerBlock);
        printf("Max shared memory per block: %zu VS sh memory used (if inserted): %zu\n", prop.sharedMemPerBlock, (BLKDIM + 2) * (BLKDIM + 2) * num_layers * sizeof(bool)); */

        for (uint64_t s = 1; s < num_steps; s++) {
            ml_gol_kernel_one_step<<<gridDim, blockDim>>>(d_current, d_next, d_layers_colors, d_combined, d_dependent, grid_size, num_layers); cudaCheckError();
            cudaDeviceSynchronize();

            swap_grids_no_ghost_kernel<<<gridDim, blockDim>>>(d_current, d_next, grid_size, num_layers);
            cudaDeviceSynchronize();

            manage_ghost_cells_kernel<<<1,1>>>(d_current, d_next, grid_size, num_layers);
            cudaDeviceSynchronize();
        }
        
        //to print last step uncomment this
        /* ml_gol_kernel_one_step<<<gridDim, blockDim>>>(d_current, d_next, d_layers_colors, d_combined, d_dependent, grid_size, num_layers); cudaCheckError();
        cudaDeviceSynchronize();

        copy_back_to_host(ml_gol, &d_current, &d_next, &d_combined, &d_dependent);

        swap_grids(ml_gol);

        create_png_for_step(ml_gol, num_steps); */
        
    }

    //cudaFree(&ml_gol);
    free_ml_gol(ml_gol);
}

void free_ml_gol(ml_gol_t* ml_gol) {
    free(ml_gol->current);
    free(ml_gol->next);
    free(ml_gol->combined);
    free(ml_gol->dependent);
    free(ml_gol);
}

void init_ml_gol_managed(ml_gol_t* ml_gol, const uint64_t grid_size, const uint64_t num_layers, const float density, const uint64_t seed) {
    srand(seed);

    ml_gol->num_layers = num_layers;
    ml_gol->grid_size = grid_size;

    uint64_t gol_grids_size = (grid_size + 2) * (grid_size + 2) * num_layers;
    cudaMallocManaged(&ml_gol->current, gol_grids_size * sizeof(bool));
    cudaMallocManaged(&ml_gol->next, gol_grids_size * sizeof(bool));


    // init non ghost cells
    for (uint64_t i = 1; i < grid_size + 1; i++) {
        for (uint64_t j = 1; j < grid_size + 1; j++) {
            for (uint16_t l = 0; l < ml_gol->num_layers; l++) {
                size_t cell_idx = idx_flat(grid_size, num_layers, i, j, l);
                ml_gol->current[cell_idx] = ((float) rand() / RAND_MAX) < density;
            }
        }
    }

    fill_ghost_cells(ml_gol->current, grid_size, num_layers);

    cudaMallocManaged(&ml_gol->layers_colors, num_layers * sizeof(color_t));
    for (uint64_t i = 0; i < ml_gol->num_layers; i++) {
        ml_gol->layers_colors[i] = get_color_for_layer(i, num_layers);
    }

    size_t size = (ml_gol->grid_size) * (ml_gol->grid_size) * sizeof(color_t);

    cudaMallocManaged(&ml_gol->combined, size * sizeof(color_t));
    cudaMallocManaged(&ml_gol->dependent, size * sizeof(color_t));

    printf("Initialized multilayer game of life with %ld layers and grid size %ld\n", num_layers, grid_size);
    
    print_layers_colors(ml_gol);
}

void init_ml_gol(ml_gol_t* ml_gol, const uint64_t grid_size, const uint64_t num_layers, const float density, const uint64_t seed) {
    srand(seed);

    ml_gol->num_layers = num_layers;
    ml_gol->grid_size = grid_size;

    uint64_t gol_grids_size = (grid_size + 2) * (grid_size + 2) * num_layers;
    ml_gol->current = (bool*) malloc(gol_grids_size * sizeof(bool));
    ml_gol->next = (bool*) malloc(gol_grids_size * sizeof(bool));

    // init non ghost cells
    for (uint64_t i = 1; i < grid_size + 1; i++) {
        for (uint64_t j = 1; j < grid_size + 1; j++) {
            for (uint16_t l = 0; l < ml_gol->num_layers; l++) {
                size_t cell_idx = idx_flat(grid_size, num_layers, i, j, l);
                ml_gol->current[cell_idx]= ((float) rand() / RAND_MAX) < density;
            }
        }
    }

    ml_gol->layers_colors = (color_t*) malloc(num_layers * sizeof(color_t));
    for (uint64_t i = 0; i < ml_gol->num_layers; i++) {
        ml_gol->layers_colors[i] = get_color_for_layer(i, num_layers);
    }

    size_t size = (ml_gol->grid_size) * (ml_gol->grid_size) * sizeof(color_t);

    ml_gol->combined = (color_t*) malloc(size);
    ml_gol->dependent = (color_t*) malloc(size);

    printf("Initialized multilayer game of life with %ld layers and grid size %ld\n", num_layers, grid_size);
    
    print_layers_colors(ml_gol);
}

void init_ml_gol_to_device(const ml_gol_t* ml_gol, bool** d_current, bool** d_next, color_t** d_layers_colors, color_t** d_combined, color_t** d_dependent) {
    // cuurent & next
    uint64_t gols_grid_size = (ml_gol->grid_size + 2) * (ml_gol->grid_size + 2) * ml_gol->num_layers;

    cudaSafeCall( cudaMalloc((void**)(d_current), gols_grid_size * sizeof(bool)));
    cudaSafeCall( cudaMalloc((void**)(d_next), gols_grid_size * sizeof(bool)));

    // layers colors (we copy the colors here becuase they never change)
    cudaSafeCall( cudaMalloc((void**)(d_layers_colors), ml_gol->num_layers * sizeof(color_t)));
    cudaSafeCall( cudaMemcpy(*d_layers_colors, ml_gol->layers_colors, ml_gol->num_layers * sizeof(color_t), cudaMemcpyHostToDevice));
    
    // combined and dependent
    cudaSafeCall( cudaMalloc((void**)(d_combined), (ml_gol->grid_size) * (ml_gol->grid_size) * sizeof(color_t)));
    cudaSafeCall( cudaMalloc((void**)(d_dependent), (ml_gol->grid_size) * (ml_gol->grid_size) * sizeof(color_t)));
}

void update_d_ml_gol(const ml_gol_t* ml_gol, bool** d_current, bool** d_next, color_t** d_combined, color_t** d_dependent) {

    // current & next
    uint64_t gols_grid_size = (ml_gol->grid_size + 2) * (ml_gol->grid_size + 2) * ml_gol->num_layers;

    cudaSafeCall( cudaMemcpy(*d_current, ml_gol->current, gols_grid_size * sizeof(bool), cudaMemcpyHostToDevice));
    cudaSafeCall( cudaMemcpy(*d_next, ml_gol->next, gols_grid_size * sizeof(bool), cudaMemcpyHostToDevice));

    // combined and dependent
    cudaSafeCall( cudaMemcpy(*d_combined, ml_gol->combined, (ml_gol->grid_size) * (ml_gol->grid_size) * sizeof(color_t), cudaMemcpyHostToDevice));
    cudaSafeCall( cudaMemcpy(*d_dependent, ml_gol->dependent, (ml_gol->grid_size) * (ml_gol->grid_size) * sizeof(color_t), cudaMemcpyHostToDevice));
}

void copy_back_to_host(const ml_gol_t* ml_gol, bool** d_current, bool** d_next, color_t** d_combined, color_t** d_dependent) {
    // current & next
    uint64_t gols_grid_size = (ml_gol->grid_size + 2) * (ml_gol->grid_size + 2) * ml_gol->num_layers;

    cudaSafeCall( cudaMemcpy(ml_gol->current, *d_current, gols_grid_size * sizeof(bool), cudaMemcpyDeviceToHost));
    cudaSafeCall( cudaMemcpy(ml_gol->next, *d_next, gols_grid_size * sizeof(bool), cudaMemcpyDeviceToHost));

    // combined and dependent
    cudaSafeCall( cudaMemcpy(ml_gol->combined, *d_combined, (ml_gol->grid_size) * (ml_gol->grid_size) * sizeof(color_t), cudaMemcpyDeviceToHost));
    cudaSafeCall( cudaMemcpy(ml_gol->dependent, *d_dependent, (ml_gol->grid_size) * (ml_gol->grid_size) * sizeof(color_t), cudaMemcpyDeviceToHost));
}

void create_all_pngs(ml_gol_t* ml_gol, bool** d_current, bool** d_next, color_t** d_layers_colors, color_t** d_combined, color_t** d_dependent, uint64_t num_steps, dim3 gridDim, dim3 blockDim) {
    /* cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);
    printf("Max threads per block: %d VS threads launched per block: %d\n", prop.maxThreadsPerBlock, blockDim.x * blockDim.y * blockDim.z);
    printf("Max blocks in each dimension: %d, %d, %d VS blocks launched: %d, %d. %d\n", prop.maxGridSize[0], prop.maxGridSize[1], prop.maxGridSize[2], gridDim.x, gridDim.y, gridDim.z);
    printf("Max registers per block: %d VS register used IDK\n", prop.regsPerBlock);
    printf("Max shared memory per block: %zu\n", prop.sharedMemPerBlock); */

    for (uint64_t s = 1; s < num_steps; s++) {
        fill_ghost_cells(ml_gol->current, ml_gol->grid_size, ml_gol->num_layers);
        
        update_d_ml_gol(ml_gol, d_current, d_next, d_combined, d_dependent);

        ml_gol_kernel_one_step<<<gridDim, blockDim>>>(*d_current, *d_next, *d_layers_colors, *d_combined, *d_dependent, ml_gol->grid_size, ml_gol->num_layers); cudaCheckError();
        cudaDeviceSynchronize();
        copy_back_to_host(ml_gol, d_current, d_next, d_combined, d_dependent);
    
        swap_grids(ml_gol);

        create_png_for_step(ml_gol, s);

        reset_combined_and_dependent(ml_gol);
    }
}

void create_png_for_step(const ml_gol_t* ml_gol, const uint64_t step) {
    create_png_for_grid(ml_gol->combined, ml_gol->grid_size, step, "combined");
    create_png_for_grid(ml_gol->dependent, ml_gol->grid_size, step, "dependent");
}

void create_png_for_grid(const color_t* grid, const uint64_t grid_size, const uint64_t step, const char* folder) {
    char filename[50];

    // 3 channels: RGB
    const uint8_t channels = 3;
    uint8_t* buffer = (uint8_t*) malloc(grid_size * grid_size * channels * sizeof(uint8_t));

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

void print_layers_colors(const ml_gol_t* ml_gol) {
    char hex[8];
    printf("Colors for the layers:\n");
    for (uint64_t i = 0; i < ml_gol->num_layers; i++) {
        color_to_hex(ml_gol->layers_colors[i], hex);
        printf("Layer %ld: %s\n", i, hex);
    }
}

void reset_combined_and_dependent(ml_gol_t* ml_gol) {
    for (uint64_t i = 0; i < ml_gol->grid_size * ml_gol->grid_size; i++) {
        ml_gol->combined[i] = BLACK;
        ml_gol->dependent[i] = BLACK;
    }
}

__host__ __device__ __forceinline__ void fill_ghost_cells(bool* current, uint64_t grid_size, uint64_t num_layers) {
    const uint64_t TOP = 1;
    const uint64_t BOTTOM = grid_size - 1;
    const uint64_t LEFT = 1;
    const uint64_t RIGHT = grid_size - 1;
    const uint64_t HALO_TOP = TOP - 1;
    const uint64_t HALO_BOTTOM = BOTTOM + 1;
    const uint64_t HALO_LEFT = LEFT - 1;
    const uint64_t HALO_RIGHT = RIGHT + 1;

    size_t src_idx;
    size_t dst_idx;
    
    // Left and right borders
    for (uint64_t i = TOP; i < BOTTOM + 1; i++) {
        src_idx = idx_flat(grid_size, num_layers, i, LEFT, 0);
        dst_idx = idx_flat(grid_size, num_layers, i, HALO_RIGHT, 0);
        memcpy(&current[dst_idx], &current[src_idx], num_layers * sizeof(bool));

        src_idx = idx_flat(grid_size, num_layers, i, RIGHT, 0);
        dst_idx = idx_flat(grid_size, num_layers, i, HALO_LEFT, 0);
        memcpy(&current[dst_idx], &current[src_idx], num_layers * sizeof(bool));
    }

    // Top and bottom borders
    for (uint64_t j = 0; j <= HALO_RIGHT; j++) {
        src_idx = idx_flat(grid_size, num_layers, TOP, j, 0);
        dst_idx = idx_flat(grid_size, num_layers, HALO_BOTTOM, j, 0);
        memcpy(&current[dst_idx], &current[src_idx], num_layers * sizeof(bool));
            
        src_idx = idx_flat(grid_size, num_layers, BOTTOM, j, 0);
        dst_idx = idx_flat(grid_size, num_layers, HALO_TOP, j, 0);
        memcpy(&current[dst_idx], &current[src_idx], num_layers * sizeof(bool));
    }
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

void swap_grids(ml_gol_t* ml_gol) {
    bool* temp = ml_gol->current;
    ml_gol->current = ml_gol->next;
    ml_gol->next = temp;
}

__device__ __forceinline__ void swap_grid_cell(bool* current, bool* next, uint64_t x, uint64_t y, uint64_t num_layers, uint64_t grid_size) {
    bool temp;
    for (uint64_t l = 0; l < num_layers; l++) {
        size_t idx = idx_flat(grid_size, num_layers, x, y, l);
        temp = current[idx];
        current[idx] = next[idx];
        next[idx] = temp;
    }
}

__host__ __device__ size_t idx(const ml_gol_t* ml_gol, uint64_t i, uint64_t j) {
    return (i * (ml_gol->grid_size + 2)) + j;
}

__host__ __device__ __forceinline__ size_t idx_flat(const uint64_t grid_size, uint64_t num_layers, uint64_t i, uint64_t j, uint64_t layer) {
    return (((i * (grid_size + 2)) + j) * num_layers) + layer;
}

__device__ __forceinline__ uint8_t count_layer_alive_neighbors(bool* current, uint64_t grid_size, uint64_t num_layers, uint64_t i, uint64_t j, uint64_t layer) {
    return  current[idx_flat(grid_size, num_layers, i - 1, j - 1, layer)] + current[idx_flat(grid_size, num_layers, i - 1, j, layer)] + current[idx_flat(grid_size, num_layers, i - 1, j + 1, layer)] +
            current[idx_flat(grid_size, num_layers, i, j - 1,     layer)] +                                                              current[idx_flat(grid_size, num_layers, i, j + 1    , layer)] +
            current[idx_flat(grid_size, num_layers, i + 1, j - 1, layer)] + current[idx_flat(grid_size, num_layers, i + 1, j, layer)] + current[idx_flat(grid_size, num_layers, i + 1, j + 1, layer)];
}

__device__ __forceinline__ color_t add_colors_device(const color_t c1, const color_t c2) {
    color_t result;
    result.r = (c1.r + c2.r) > 255 ? 255 : c1.r + c2.r;
    result.g = (c1.g + c2.g) > 255 ? 255 : c1.g + c2.g;
    result.b = (c1.b + c2.b) > 255 ? 255 : c1.b + c2.b;
    return result;
}

__global__ void ml_gol_kernel_one_step(bool* d_current, bool* d_next, color_t* d_layers_colors, color_t* d_combined, color_t* d_dependent, uint64_t grid_size, uint64_t num_layers) {
    //extern __shared__ ml_cell_t blk_cells[];
    
    // actual index of grid with NO ghost cells
    uint64_t x = threadIdx.x + (blockDim.x * blockIdx.x);
    uint64_t y = threadIdx.y + (blockDim.y * blockIdx.y);

    size_t no_ghost_idx = x * grid_size + y;

    // since we have ghost cells, we need to avoid them
    // this should do the trick
    x++; y++;

    uint16_t tot_alive_neighbors = 0;

    // reset combined and dependent
    d_combined[no_ghost_idx] = BLACK;
    d_dependent[no_ghost_idx] = BLACK;

    for (uint64_t l = 0; l < num_layers; l++) {
        uint8_t alive_neighbors = count_layer_alive_neighbors(d_current, grid_size, num_layers, x, y, l);
        tot_alive_neighbors += alive_neighbors;

        // The state of the current cell
        bool is_alive = d_current[idx_flat(grid_size, num_layers, x, y, l)];
        tot_alive_neighbors += (uint16_t) is_alive;

        // The state of the current cell in the next step based on the rules of the game of life
        bool next_state = (is_alive && !(alive_neighbors < 2 || alive_neighbors > 3)) || (!is_alive && alive_neighbors == 3);

        d_next[idx_flat(grid_size, num_layers, x, y, l)] = next_state;
        
        // COMBINED
        if (next_state) d_combined[no_ghost_idx] = add_colors_device(d_combined[no_ghost_idx], d_layers_colors[l]);
    }

    //DEPENDENT
    uint8_t channel_value = (uint8_t) ((((float) tot_alive_neighbors) / 9) * 255);
    d_dependent[no_ghost_idx] = (color_t){channel_value, channel_value, channel_value};
}

__global__ void swap_grids_no_ghost_kernel(bool* d_current, bool* d_next, uint64_t grid_size, uint64_t num_layers) {
    // actual index of grid with NO ghost cells
    uint64_t x = threadIdx.x + (blockDim.x * blockIdx.x);
    uint64_t y = threadIdx.y + (blockDim.y * blockIdx.y);

    x++; y++;

    //swap grids (one cell per thread) - no ghost swap
    swap_grid_cell(d_current, d_next, x, y, num_layers, grid_size);        
}

__global__ void manage_ghost_cells_kernel(bool* d_current, bool* d_next, uint64_t grid_size, uint64_t num_layers) {
    // one thread swaps ghost cells and then update them
        for (uint64_t row = 0; row <= grid_size + 1; row++) {
            swap_grid_cell(d_current, d_next, row, 0, num_layers, grid_size);
            swap_grid_cell(d_current, d_next, row, grid_size + 1, num_layers, grid_size);   
        }

        for (uint64_t column = 1; column <= grid_size; column++) {
            swap_grid_cell(d_current, d_next, 0, column, num_layers, grid_size);
            swap_grid_cell(d_current, d_next, grid_size + 1, column, num_layers, grid_size);   
        }
        
        fill_ghost_cells(d_current, grid_size, num_layers);
}
