CC	     := g++
CXX      := c++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror
LDFLAGS  := -L/usr/lib -lstdc++
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   :=
INCLUDE  := -Iinclude/

all:
	g++ chip8.cpp opcodes.cpp main.cpp -o chip8

$(TARGET): $(TARGET).c
	$(CC) $(CXXFLAGS) $(INCLUDE) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
