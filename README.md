# PDC - Multilayer Game of Life
Project for the course on **Parallel and Distributed Computing**, with implementations of **Multilayer Game of Life** in **OpenMP**, **Rust** and **Cuda**.

## 🔎 About
There are **N layers**, each with an instance of the **game of life** (with traditional rules) plus a layer whose cells depend on the value of the other layers. 
At each step it returns two matrices, one representing the first N layers (**combined matrix**) and one representing the dependent layer (**dependent matrix**).
The combined matrix simultaneously represents all game-of-life layers with a combination of colors, while the dependent matrix will have the value of cells with rules based on the other layers.

## 🔗 Dependencies
To run this executible you need to install [this](https://github.com/LuaDist/libpng/blob/master/INSTALL) library to create pngs. 

## 🔵 OpenMP version
### 🛠️ Build
To build the OpenMP version move to the `openmp` directory:
```bash
cd openmp
```
Then you can create the obj files and the executable using:
```bash
make
```
The obj files are located in `openmp/obj` while the executable is in the folder `openmp/bin`.

You can clean the obj and bin directories using:
```bash
make clean
```

### ▶️ Execute
To execute the OpenMp version move to the openmp directory and then run:
```bash
./bin/multilayer-game-of-life
```
You can specify extra parameters:
```bash
./bin/multilayer-game-of-life <grid_size> <num_layers> <num_steps> <create_png> <density> <seed>
```
`<grid_size>` is the size of the grids (layers, combined and dependent), default 128.

`<num_layers>` is the number of layers, default 3.

`<num_steps>` the max number of steps, default 64.

`<create_png>` whether to create pngs for each step, default true.

`<density>` the initial density of the grids when generated, default 0.3.

`<seed>` the seed used for the random number generator, the default uses the **time()** function from **time.h**.

## 🟠 Rust version
## 🟢 Cuda version
### 🛠️ Build
To build the CUDA version move to the `cuda` directory:
```bash
cd cuda
```
Then you can create the obj files and the executable using:
```bash
make release
```
The obj files are located in `cuda/obj` while the executable is in the folder `cuda/bin`.

You can clean the obj and bin directories using:
```bash
make clean
```

### ▶️ Execute
To execute the CUDA version move to the cuda directory and then run:
```bash
./bin/multilayer-game-of-life
```
You can specify extra parameters:
```bash
./bin/multilayer-game-of-life <grid_size> <num_layers> <num_steps> <create_png> <use_shared> <density> <seed>
```
`<grid_size>` is the size of the grids (layers, combined and dependent), default 128.

`<num_layers>` is the number of layers, default 3.

`<num_steps>` the max number of steps, default 64.

`<create_png>` whether to create pngs for each step, default true.

`<use_shared>` whether use device shared memory, default false.

`<density>` the initial density of the grids when generated, default 0.3.

`<seed>` the seed used for the random number generator, the default uses the **time()** function from **time.h**.
