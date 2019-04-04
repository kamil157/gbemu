#ifndef CPU_H
#define CPU_H

#include "mmu.h"
#include "utils.h"

#include <bitset>
#include <cstdint>
#include <memory>
#include <ostream>
#include <vector>

#include <nlohmann/json.hpp>

class Cpu {
public:
    Cpu(const byteCodePtr& code, std::unique_ptr<Mmu> mmu);
    bool runCommand();
    uint16_t getPc() const;
    std::string toString() const;

private:
    byteCodePtr code;

    uint8_t a = 0; // Accumulator
    std::bitset<8> f; // Status flags

    // General purpose registers
    union {
        struct {
            uint8_t c;
            uint8_t b;
        };
        uint16_t bc;
    };

    union {
        struct {
            uint8_t e;
            uint8_t d;
        };
        uint16_t de;
    };

    union {
        struct {
            uint8_t l;
            uint8_t h;
        };
        uint16_t hl;
    };

    uint16_t sp = 0; // Stack Pointer
    uint16_t pc = 0; // Program Counter

    int cycles = 0;
    std::unique_ptr<Mmu> mmu;

    // Set flag in register f to b.
    void setFlag(uint8_t flag, bool b);
    void setSP(uint8_t lo, uint8_t hi);
    void setPC(uint8_t lo, uint8_t hi);
    bool runExtendedCommand();
    uint8_t read();
    // Push nn to stack.
    void push(uint16_t nn);
    void setFlagsFromJson(nlohmann::json opcode);
    // Rotate reg left, put old bit 7 in flag C and set Z if result is zero.
    void rotateLeft(uint8_t& reg);
    void relativeJump(uint8_t flag);
    void call();
};

#endif // CPU_H
