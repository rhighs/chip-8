#include "chip8.hpp"
#include <fstream>

const unsigned int START_ADDR = 0x200;

void Chip8::load_rom(const char* filename){


    std::ifstream file(filename, std::ios::binary, std::ios::ate);

    if(file.is_open()){

        //allocates a buffer to held the rom
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        //reads and saves to buffer
        file.seekg(0, ios::beg);
        file.read(buffer, size);

        for(long i = 0; i < size; ++i)
            memory[START_ADDR + i] = buffer[i];

        //unalloc buffer
        delete[] buffer;
    }

}
