#include "../include/chip8.hpp"
#include <fstream>

const unsigned int START_ADDR = 0x200;
const unsigned int FONTSET_SIZE = 80;

const unsigned int FONTSET_START_ADDR = 0x50;

uint8_t fontset[FONTSET_SIZE] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() : rand_gen(std::chrono::system_clock::now().time_since_epoch().count()){
    pc = START_ADDR;

    for(uint32_t i = 0; i < FONTSET_SIZE; ++i)
        memory[FONTSET_START_ADDR + i] = fontset[i];

    rand_byte = std::uniform_int_distribution<uint8_t>(0, 255u);
}

uint8_t right_most_byte(){
    return (opcode & 0x00FFu); 
}

uint8_t x(){
    return (opcode & 0x0F00u) >> 8u;
}

uint8_t y(){
    return (opcode & 0x00F0u) >> 8u;
}

void Chip8::load_rom(char const* filename){
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if(!file.is_open()) return;

    //allocates a buffer to held the rom
    std::streampos size = file.tellg();
    char* buffer = new char[size];

    //reads and saves to buffer
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);

    for(long i = 0; i < size; ++i)
        memory[START_ADDR + i] = buffer[i];

    //unalloc buffer
    delete[] buffer;
}

/*
    in order to get the address in which the opcode operates,
    we need to do AND it with 0x0FFF pretty obvious...
 */

//cls opcode reset video
void Chip8::op_00E0(){
    memset(video, 0, sizeof(video));
}

//RET return from a function
void Chip8::op_00EE(){
    --sp;
    pc = stack[sp];
}

//jump to adress
void Chip8::op_1nnn(){
    uint16_t address = opcode & 0x0FFFu;

    pc = address;
}

/*
    this is a CALL instruction,
    to prevent infinite recursion we need to set the current sp to the next instruction. 
    to proceed "call" we just need to set the pc to the address given in the opcode 
*/

void Chip8::op_2nnn(){
    uint16_t address = opcode & 0x0FFFu;
    
    stack[sp] = pc;
    ++sp;
    
    pc = address;
}

/*
    skip next instruction if value at register[x] is equal to the rr byte given 

*/
void Chip8::op_3xrr(){

    if(registers[x()] == right_most_byte())
        pc += 2;
}

/*skip next instruction if the value at registers[x] differs from rr*/

void Chip8::op_4xrr(){

    if(registers[x()] != right_most_byte())
        pc+=2;
}

void Chip8::op_5xy0(){
    if(registers[x()] == registers[y()])
        pc+=2;
}

void Chip8::op_6xrr(){
    registers[x()] = right_most_byte();
}

void Chip8::op_7xrr(){
    registers[x()] += right_most_byte();
}

void Chip8::op_8xy0(){
    registers[y()] = registers[x()]; 
}

void Chip8::op_8xy1(){
    uint8_t x = x();

    registers[x] = registers[y()] || registers[x];
}
