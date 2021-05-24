CC	     := g++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror
LDFLAGS  := -L/usr/lib -lstdc++
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   :=
INCLUDE  := -Iinclude/

SRC_FILES = ./src/chip8.cpp ./src/opcodes.cpp ./src/main.cpp
O_FILES = $(SRC_FILES:%.cc=%.o)

all:
	$(CC) $(INCLUDE) $(SRC_FILES) -o chip8

clean:
	$(RM) $(TARGET)
