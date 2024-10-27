# Makefile for Snake Game

# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -lcurses

# Target executable
TARGET = snake
SRC = snake_game.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean up build files
clean:
	rm -f $(TARGET)
