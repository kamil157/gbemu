#ifndef CPU_H
#define CPU_H

#include "mmu.h"
#include "utils.h"

#include <bitset>
#include <cstdint>
#include <memory>
#include <ostream>
#include <vector>

class Cpu {
public:
    Cpu(const byteCodePtr& code, std::unique_ptr<Mmu> mmu);
    bool runCommand();

private:
    byteCodePtr code;

    uint8_t a = 0; // Accumulator
    std::bitset<8> f; // Status flags

    // General purpose registers
    uint8_t b = 0;
    uint8_t c = 0;
    uint8_t d = 0;
    uint8_t e = 0;
    uint8_t h = 0;
    uint8_t l = 0;

    uint16_t sp = 0; // Stack Pointer
    uint16_t pc = 0; // Program Counter

    int cycles = 0;
    std::unique_ptr<Mmu> mmu;

    // Load operand into register
    uint8_t ld(uint8_t& reg);

    // Set flag in register f to b
    void setFlag(uint8_t flag, bool b);
    void setHL(uint8_t hi, uint8_t lo);
    void setHL(uint16_t nn);
    uint16_t getHL();
    void decrementHL();
    void setSP(uint8_t hi, uint8_t lo);
    bool runExtendedCommand();
    friend std::ostream& operator<<(std::ostream& os, Cpu const& cpu);
};

#endif // CPU_H
