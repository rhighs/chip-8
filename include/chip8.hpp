#include <cstdint>
#include <chrono>
#include <random>

#include <string.h>

class Chip8{
    public:
        std::default_random_engine rand_gen;
        std::uniform_int_distribution<uint8_t> rand_byte;

        Chip8();
        uint8_t registers[16]{};
        uint8_t memory[4096]{};
        uint16_t index{};
        uint16_t pc{};
        uint16_t stack[16]{};
        uint8_t sp{};
        uint8_t delay_timer{};
        uint8_t sound_timer{};
        uint8_t keypad[16]{};
        void init_funtables();

        typedef void (Chip8::*funptr)();

        funptr tab[100];

        /*each value is either set to 1 or 0*/
        uint32_t video[64 * 32]{};
        uint16_t opcode;
        void load_rom(char const* filename);

        /*opcodes*/
        uint8_t rightmost_byte();
        uint8_t x();
        uint8_t y();

        void op_00E0();
        void op_00EE();
        void op_1nnn();
        void op_2nnn();
        void op_3xrr();
        void op_4xrr();
        void op_5xy0();
        void op_6xrr();
        void op_7xrr();
        void op_8xy0();
        void op_8xy1();
        void op_8xy2();
        void op_8xy3();
        void op_8xy4();
        void op_8xy5();
        void op_8x06();
        void op_8xy7();
        void op_8xyE();
        void op_9xy0();
        void op_Annn();
        void op_Bnnn();
        void op_Cxrr();
        void op_Dxyn();
        void op_Ex9e();
        void op_Exa1();
        void op_Fx07();
        void op_Fx0a();
        void op_Fx15();
        void op_Fx18();
        void op_Fx1e();
        void op_Fx29();
        void op_Fx33();
        void op_Fx55();
        void op_Fx65();
};
