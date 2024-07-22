# Makefile for the OpenMP and CUDA version of the Multilayer Game of Life
# 
# Move to the directory where this Makefile is located and run:
# make
#
# To clean the directory, run:
# make clean
#
# To run the program, run:
# bin/multilayer-game-of-life 
# Check the README.md file for more information.
#

# directories
INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Compiler for C files
CC = gcc

# Compilation flags for C files
CFLAGS = -Wall -Wextra -I$(INC_DIR) $(shell pkg-config --cflags libpng) -lm -O3 -march=native

# Compilation flags for CUDA files
NVCCFLAGS = -O3 -arch=sm_86 -Xcompiler -Wall,-Wextra -I$(INC_DIR)

# Linker flags
LDFLAGS = $(shell pkg-config --libs libpng) -lm

# Target executable
TARGET = $(BIN_DIR)/multilayer-game-of-life

# sources and objects
CSRCS = $(wildcard $(SRC_DIR)/*.c)
CU_SRCS = $(wildcard $(SRC_DIR)/*.cu)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(CSRCS)) $(patsubst $(SRC_DIR)/%.cu, $(OBJ_DIR)/%.o, $(CU_SRCS))

# rules
all: directories $(TARGET)

directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(TARGET): $(OBJS)
	nvcc $(OBJS) $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cu
	mkdir -p $(OBJ_DIR)
	nvcc $(NVCCFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)

.PHONY: all clean directories
