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
    bool runExtendedCommand();
    uint8_t read();
    uint16_t read16();
    // Decrement reg.
    void dec(uint8_t& reg);
    // Increment reg.
    void inc(uint8_t& reg);
    // xor a with reg, store result in a.
    void xorA(uint8_t reg);
    // Pop nn from stack.
    uint16_t pop();
    // Push nn to stack.
    void push(uint16_t nn);
    void setFlagsFromJson(nlohmann::json opcode);
    // Rotate reg left, put old bit 7 in flag C and set Z if result is zero.
    void rotateLeft(uint8_t& reg);
    void relativeJump(bool condition);
    void call();
    void cp(uint8_t n);
    void sub(uint8_t n);
};

#endif // CPU_H
