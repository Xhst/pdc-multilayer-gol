# PDC - Multilayer Game of Life
Project for the course on **Parallel and Distributed Computing**, with implementations of **Multilayer Game of Life** in **Open MP** and **Cuda**.

## 🔎 About
There are **N layers**, each with an instance of the **game of life** (with traditional rules) plus a layer whose cells depend on the value of the other layers. 
At each step it returns two matrices, one representing the first N layers (**combined matrix**) and one representing the dependent layer (**dependent matrix**).
The combined matrix simultaneously represents all game-of-life layers with a combination of colors, while the dependent matrix will have the value of cells with rules based on the other layers.