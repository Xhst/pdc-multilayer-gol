# PDC - Multilayer Game of Life
Project for the course on **Parallel and Distributed Computing**, with implementations of **Multilayer Game of Life** in **OpenMP** and **Cuda**.

## 🔎 About
There are **N layers**, each with an instance of the **game of life** (with traditional rules) plus a layer whose cells depend on the value of the other layers. 
At each step it returns two matrices, one representing the first N layers (**combined matrix**) and one representing the dependent layer (**dependent matrix**).
The combined matrix simultaneously represents all game-of-life layers with a combination of colors, while the dependent matrix will have the value of cells with rules based on the other layers.

## 🟠 OpenMP version
### 🛠️ Build
To build the OpenMP version move to the `openmp` directory:
```
cd openmp
```
Then you can create the obj files and the executable using:
```
make
```
The obj files are located in `openmp/obj` while the executable is in the folder `openmp/bin`.

You can clean the obj and bin directories using:
```
make clean
```

### ▶️ Execute
To execute the OpenMp version move to the openmp directory and then run:
```
./bin/multilayer-game-of-life
```
You can specify extra parameters:
```
./bin/multilayer-game-of-life <grid_size> <num_layers> <num_steps> <density> <seed>
```
`<grid_size>` is the size of the grids (layers, combined and dependent), default 128.

`<num_layers>` is the number of layers, default 3.

`<num_steps>` the max number of steps, default 64.

`<density>` the initial density of the greed when generated, default 0.3.

`<seed>` the seed used for the random number generator, the default uses the **time()** function from **time.h**.

## 🟢 Cuda Version