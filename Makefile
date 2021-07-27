CC	     := g++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror
LDFLAGS  := -L/usr/lib -lstdc++ -lSDL2
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := chip8
INCLUDE  := -Iinclude/

SRC_FILES = ./src/chip8.cpp ./src/opcodes.cpp ./src/video.cpp ./src/main.cpp
O_FILES = $(SRC_FILES:%.cc=%.o)

all:
	$(CC) $(INCLUDE) $(LDFLAGS) $(SRC_FILES) -o ${TARGET}

clean:
	$(RM) $(TARGET)
