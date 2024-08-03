# Makefile for the OpenMP and CUDA version of the Multilayer Game of Life
# 
# Move to the directory where this Makefile is located and run:
# make release
# make debug
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
OBJ_DIR_DEBUG = obj_debug
BIN_DIR = bin

# Compiler for C files
CC = gcc

# Debug flags
DEBUG_FLAGS = -g -G -O0

# Release flags
RELEASE_FLAGS = -O3

# Compilation flags for C files
CFLAGS = -Wall -Wextra -I$(INC_DIR) $(shell pkg-config --cflags libpng) -lm

# Compilation flags for CUDA files
NVCCFLAGS = -arch=native --ptxas-options=-v -Xcompiler -Wall,-Wextra -I$(INC_DIR)

# Linker flags
LDFLAGS = $(shell pkg-config --libs libpng) -lm

# Target executable
TARGET = $(BIN_DIR)/multilayer-game-of-life
TARGET_DEBUG = $(BIN_DIR)/multilayer-game-of-life-debug

# Sources and objects
CSRCS = $(wildcard $(SRC_DIR)/*.c)
CU_SRCS = $(wildcard $(SRC_DIR)/*.cu)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(CSRCS)) $(patsubst $(SRC_DIR)/%.cu, $(OBJ_DIR)/%.o, $(CU_SRCS))
OBJS_DEBUG = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR_DEBUG)/%.o, $(CSRCS)) $(patsubst $(SRC_DIR)/%.cu, $(OBJ_DIR_DEBUG)/%.o, $(CU_SRCS))

# Rules
release: directories $(TARGET)

debug: directories_debug $(TARGET_DEBUG)

directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

directories_debug:
	mkdir -p $(OBJ_DIR_DEBUG) $(BIN_DIR)

$(TARGET): $(OBJS)
	nvcc $(OBJS) $(LDFLAGS) -o $@

$(TARGET_DEBUG): $(OBJS_DEBUG)
	nvcc $(OBJS_DEBUG) $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cu
	nvcc $(NVCCFLAGS) $(RELEASE_FLAGS) -c -o $@ $<

$(OBJ_DIR_DEBUG)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR_DEBUG)/%.o: $(SRC_DIR)/%.cu
	nvcc $(NVCCFLAGS) $(DEBUG_FLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(OBJ_DIR_DEBUG) $(BIN_DIR)

.PHONY: all clean directories directories_debug
