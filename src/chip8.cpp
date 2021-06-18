#include "chip8.hpp"
#include <fstream>

using namespace sku;
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
    init_funtables();
}

void Chip8::load_rom(char const* filename){
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if(!file.is_open()) return;
    std::streampos size = file.tellg();
    char* buffer = (char *)&(memory[START_ADDR + 1]);
    //reads and saves to buffer
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
}

void Chip8::cycle() {
    opcode = (memory[pc] << 8u) | memory[pc + 1];
    pc += 2;
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();
    if(delay_timer) --delay_timer;
    if(sound_timer) --sound_timer;
}

void Chip8::init_funtables(){
    //setting up a fun pointers table as suggezted in tha guide
    tab[0x1] = &Chip8::op_1nnn;
    tab[0x2] = &Chip8::op_2nnn;
    tab[0x3] = &Chip8::op_3xrr;
    tab[0x4] = &Chip8::op_4xrr;
    tab[0x5] = &Chip8::op_5xy0;
    tab[0x6] = &Chip8::op_6xrr;
    tab[0x7] = &Chip8::op_7xrr;
    tab[0x9] = &Chip8::op_9xy0;
    tab[0xA] = &Chip8::op_Annn;
    tab[0xB] = &Chip8::op_Bnnn;
    tab[0xC] = &Chip8::op_Cxrr;
    tab[0xD] = &Chip8::op_Dxyn;
    //funs to other nested tables
    tab[0x0] = Chip8::get_tab0();
    tab[0x8] = Chip8::get_tab8();
    tab[0xE] = Chip8::get_tabE();
    tab[0xF] = Chip8::get_tabF();

    tab0[0x0] = &Chip8::op_00E0;
    tab0[0xE] = &Chip8::op_00EE;

    tab8[0x0] = &Chip8::op_8xy0;
    tab8[0x1] = &Chip8::op_8xy1;
    tab8[0x2] = &Chip8::op_8xy2;
    tab8[0x3] = &Chip8::op_8xy3;
    tab8[0x4] = &Chip8::op_8xy4;
    tab8[0x5] = &Chip8::op_8xy5;
    tab8[0x6] = &Chip8::op_8x06;
    tab8[0x7] = &Chip8::op_8xy7;
    tab8[0xE] = &Chip8::op_8xyE;

    tabE[0x1] = &Chip8::op_ExA1;
    tabE[0xE] = &Chip8::op_Ex9E;
   
    tabF[0x07] = &Chip8::op_Fx07;
    tabF[0x0A] = &Chip8::op_Fx0A;
    tabF[0x15] = &Chip8::op_Fx15;
    tabF[0x18] = &Chip8::op_Fx18;
    tabF[0x1E] = &Chip8::op_Fx1E;
    tabF[0x29] = &Chip8::op_Fx29;
    tabF[0x33] = &Chip8::op_Fx33;
    tabF[0x55] = &Chip8::op_Fx55;
    tabF[0x65] = &Chip8::op_Fx65;
}

void Chip8::table_0(){(this->*(tab0[opcode & 0x000Fu]))();}
void Chip8::table_8(){(this->*(tab8[opcode & 0x000Fu]))();}
void Chip8::table_E(){(this->*(tabE[opcode & 0x000Fu]))();}
void Chip8::table_F(){(this->*(tabF[opcode & 0x00FFu]))();}

