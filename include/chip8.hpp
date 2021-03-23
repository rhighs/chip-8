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

        void load_rom(char const* filename);

        /*opcodes*/
        void op_00E0();
        void op_00EE();
        void op_1nnn();

        /*each value is either set to 1 or 0*/
        uint32_t video[64 * 32]{};
        uint16_t opcode;
};
