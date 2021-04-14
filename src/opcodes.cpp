#include "../include/chip8.hpp"

uint8_t rightmost_byte(){
    return (opcode & 0x00FFu); 
}

uint8_t x(){
    return (opcode & 0x0F00u) >> 8u;
}

uint8_t y(){
    return (opcode & 0x00F0u) >> 8u;
}

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
