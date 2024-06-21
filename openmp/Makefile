# Makefile for the OpenMP version of the Multilayer Game of Life
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

# Compiler
CC = gcc

# Compilation flags
CFLAGS = -Wall -Wextra -I$(INC_DIR) $(shell pkg-config --cflags libpng) -lm -fopenmp

# Linker flags
LDFLAGS = $(shell pkg-config --libs libpng) -lm -fopenmp

# Target executable
TARGET = $(BIN_DIR)/multilayer-game-of-life

# sources and objects
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# rules
all: directories $(TARGET)

directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)

.PHONY: all clean directories