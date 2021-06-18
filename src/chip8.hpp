#include <cstdint>
#include <chrono>
#include <random>

#include <string.h>

namespace sku {
    class Chip8{
        public:
            std::default_random_engine rand_gen;
            std::uniform_int_distribution<uint8_t> rand_byte;

            const int VIDEO_HEIGHT = 600;
            const int VIDEO_WIDTH = 800;
            uint8_t FONT_START_ADDRESS = 0xF;

            Chip8();
            uint8_t sp{};
            uint8_t keypad[16]{};
            uint8_t delay_timer{};
            uint8_t sound_timer{};
            uint8_t memory[4096]{};
            uint8_t registers[16]{};
            uint16_t pc{};
            uint16_t index{};
            uint16_t stack[16]{};

            void table_0();
            void table_E();
            void table_8();
            void table_F();
            void NOOP(){return;}
            void init_funtables();
            typedef void (Chip8::*funptr)();
            funptr tab[0xF + 1]{&Chip8::NOOP};
            funptr tab0[0xE + 1]{&Chip8::NOOP};
            funptr tab8[0xE + 1]{&Chip8::NOOP};
            funptr tabE[0xE + 1]{&Chip8::NOOP};
            funptr tabF[0x65 + 1]{&Chip8::NOOP};
            inline funptr get_tab0(){return *tab0;};
            inline funptr get_tab8(){return *tab8;};
            inline funptr get_tabE(){return *tabE;};
            inline funptr get_tabF(){return *tabF;};

            //each value is either set to 1 or 0
            uint32_t video[64 * 32]{};
            uint16_t opcode;
            void load_rom(char const* filename);

            //opcodes utils
            uint8_t rightmost_byte();
            uint8_t x();
            uint8_t y();
            uint8_t negate(uint8_t byte);

            //opcodes
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
            void op_ExA1();
            void op_Ex9E();
            void op_Fx07();
            void op_Fx0A();
            void op_Fx15();
            void op_Fx18();
            void op_Fx1A();
            void op_Fx1E();
            void op_Fx29();
            void op_Fx33();
            void op_Fx55();
            void op_Fx65();

        private: 
    };
}

