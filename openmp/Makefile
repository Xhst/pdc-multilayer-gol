# directories
INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

CC = gcc
CFLAGS = -Wall -Wextra -I$(INC_DIR)
LDFLAGS =

TARGET = $(BIN_DIR)/multilayer-game-of-life


SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)

.PHONY: all clean
