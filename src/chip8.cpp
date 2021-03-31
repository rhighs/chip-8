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
    tab[0xC] = &Chip8::op_Cxkk;
    tab[0xD] = &Chip8::op_Dxyn;

    //funs to other nested tables
    tab[0x0] = &Chip8::get_tab0;
    tab[0x8] = &Chip8::get_tab8;
    tab[0xE] = &Chip8::get_tabE;
    tab[0xF] = &Chip8::get_tabF;
}

uint8_t rightmost_byte(){
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

    std::streampos size = file.tellg();
    char* buffer = (char *)&(memory[START_ADDR + 1]);

    //reads and saves to buffer
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
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

    if(registers[x()] == rightmost_byte())
        pc += 2;
}

/*skip next instruction if the value at registers[x] differs from rr*/

void Chip8::op_4xrr(){

    if(registers[x()] != rightmost_byte())
        pc+=2;
}

void Chip8::op_5xy0(){
    if(registers[x()] == registers[y()])
        pc+=2;
}

void Chip8::op_6xrr(){
    registers[x()] = rightmost_byte();
}

void Chip8::op_7xrr(){
    registers[x()] += rightmost_byte();
}

void Chip8::op_8xy0(){
    registers[y()] = registers[x()]; 
}

void Chip8::op_8xy1(){
    uint8_t x = x();

    registers[x] = registers[y()] || registers[x];
}

void Chip8::op_8xy2(){
    uint8_t x = x();

    registers[x] = registers[y()] & registers[x];
}

void Chip8::op_8xy3(){
    uint8_t x = x();

    registers[x] = registers[y()] ^ registers[x];
}

void Chip8::op_8xy4()
{
    uint8_t x = x();
    uint8_t y = y();

    uint16_t sum = registers[x] + registers[y];

    if (sum > 255U)
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[x] = sum & 0xFFu;
}

void Chip8::op_8xy5(){
    uint8_t x = x();
    uint8_t y = y();

    if (registers[x] > registers[y])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[x] -= registers[y];
}

void Chip8::op_8x06(){
    uint8_t x = x();

    register[0xF] = register[x] & 0x1u;

    registers[x] >>= 1u;
}

void Chip8::op_8xy7(){
    uint8_t x = x();
    uint8_t y = y();

    if (registers[y] > registers[x])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[x] = registers[y] - registers[x];
}

void Chip8::op_8xyE(){
    uint8_t x = x();
    uint8_t y = y();

    registers[0xF] = registers[x] & 128u >> 7u;
    registers[x] <<= 1;
}

void Chip8::op_9xy0(){
    uint8_t x = x();
    uint8_t y = y();

    if(registers[x] != registers[y]){
        pc += 2;
    }
}

void Chip8::op_Annn(){
    uint16_t address = opcode & 0x0FFFu;

    index = address;
}

void Chip8::op_Bnnn(){
    uint16_t address = opcode & 0x0FFFu;

    pc = registers[0] + address;
}

void Chip8::op_Cxrr(){
    registers[x()] = randByte(randGen) & rightmost_byte();
}

void Chip8::op_Dxyn(){
    uint8_t x = x();
    uint8_t y = y();

    uint8_t height = opcode & 0x000Fu;

    auto xpos = registers[x] % VIDEO_WIDTH;
    auto ypos = registers[y] % VIDEO_HEIGHT;
    register[0xF] = 0;

    for(uint8_t row = 0; row < height; ++row){
        uint8_t sprite_byte = memory[index + row];

        for(uint8_t col = 0; col < 8; ++col){
            uint8_t sprite_pixel = sprite_byte & (0x80u >> col);
            uint32_t* screen_pixel = &video[(ypos + row) * VIDEO_WIDTH + (xpos + col)];

            if (screen_pixel){
                //mark collision
                if (*screen_pixel == 0xFFFFFFFF)
                    registers[0xF] = 1;

                *screen_pixel ^= 0xFFFFFFFF;       
            }
        }
    }
}

//input key based instructions
void Chip8::op_Ex9e(){
    auto key = registers[x()];

    if(keypad[key]){
        pc += 2;
    }
}

void Chip8::op_Exa1(){
    auto key = registers[x()];

    if(!keypad[key]){
        pc += 2;
    }
}

void Chip8::op_Fx07(){
    registers[x()] = delayTimer;
}

//wait for a key to be pressed
void Chip8::op_Fx0a(){
    for(uint8_t = 0; i < 16; ++i){
        if(keypad[i]){
            registers[x()] = i;
            return;
        }
    }

    pc -= 2;
}

void Chip8::op_Fx15(){
    delayTimer = registers[x()];
}

void Chip8::op_Fx18(){
    soundTimer = registers[x()];
}

void Chip8::op_Fx1e(){
    index += registers[x()];
}

void Chip8::op_Fx29(){
    uint8_t digit = registers[x()];
    index = FONT_START_ADDRESS + (5 * digit);
}

void Chip8::op_Fx33(){
    uint8_t value = registers[x()];

    memory[index + 2] = value % 10;
    value /= 10;

    memory[index + 1] = value % 10;
    value /= 10;

    memory[index] = value % 10;
}

void Chip8::op_Fx55(){
    for(uint8_t i = 0; i < x(); ++i)
        memory[index + 1] = registers[i]
}

void Chip8::op_Fx65(){
    for(uint8_t i = 0; i < x(); ++i)
        registers[i] = memory[index + i];
}

