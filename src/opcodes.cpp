#include "../include/chip8.hpp"

uint8_t Chip8::rightmost_byte(){
    return (opcode & 0x00FFu); 
}

uint8_t Chip8::negate(uint8_t byte)
{
    int fneg = 1;
    return ( byte ^ -fneg ) + fneg;
}

uint8_t Chip8::x(){
    return (opcode & 0x0F00u) >> 8u;
}

uint8_t Chip8::y(){
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

/*skip next instruction if the value at registers[_x] differs from rr*/

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
    uint8_t _x = x();

    registers[_x] = registers[y()] || registers[_x];
}

void Chip8::op_8xy2(){
    uint8_t _x = x();

    registers[_x] = registers[y()] & registers[_x];
}

void Chip8::op_8xy3(){
    uint8_t _x = x();

    registers[_x] = registers[y()] ^ registers[_x];
}

void Chip8::op_8xy4()
{
    uint8_t _x = x();
    uint8_t _y = y();

    uint16_t sum = registers[_x] + registers[_y];

    if (sum > 255U)
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[_x] = sum & 0xFFu;
}

void Chip8::op_8xy5(){
    uint8_t _x = x();
    uint8_t _y = y();

    if (registers[_x] > registers[_y])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[_x] -= registers[_y];
}

void Chip8::op_8x06(){
    uint8_t _x = x();

    registers[0xF] = registers[_x] & 0x1u;

    registers[_x] >>= 1u;
}

void Chip8::op_8xy7(){
    uint8_t _x = x();
    uint8_t _y = y();

    if (registers[_y] > registers[_x])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[_x] = registers[_y] - registers[_x];
}

void Chip8::op_8xyE(){
    uint8_t _x = x();
    uint8_t _y = y();

    registers[0xF] = registers[_x] & 128u >> 7u;
    registers[_x] <<= 1;
}

void Chip8::op_9xy0(){
    uint8_t _x = x();
    uint8_t _y = y();

    if(registers[_x] != registers[_y]){
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
    registers[x()] = rand_byte(rand_gen) & rightmost_byte();
}

void Chip8::op_Dxyn(){
    uint8_t _x = x();
    uint8_t _y = y();

    uint8_t height = opcode & 0x000Fu;

    auto xpos = registers[_x] % VIDEO_WIDTH;
    auto ypos = registers[_y] % VIDEO_HEIGHT;
    registers[0xF] = 0;

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
void Chip8::op_Ex9E(){
    auto key = registers[x()];

    if(keypad[key]){
        pc += 2;
    }
}

void Chip8::op_ExA1(){
    auto key = registers[x()];

    if(!keypad[key]){
        pc += 2;
    }
}

void Chip8::op_Fx07(){
    registers[x()] = delay_timer;
}

//wait for a key to be pressed
void Chip8::op_Fx0A(){
    for(uint8_t i = 0; i < 16; ++i){
        if(keypad[i]){
            registers[x()] = i;
            return;
        }
    }

    pc -= 2;
}

void Chip8::op_Fx15(){
    delay_timer = registers[x()];
}

void Chip8::op_Fx18(){
    sound_timer = registers[x()];
}

void Chip8::op_Fx1E(){
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
        memory[index + 1] = registers[i];
}

void Chip8::op_Fx65(){
    for(uint8_t i = 0; i < x(); ++i)
        registers[i] = memory[index + i];
}
