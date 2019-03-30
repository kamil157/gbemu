#ifndef CPU_H
#define CPU_H

#include "mmu.h"

#include <cstdint>
#include <vector>
#include <ostream>
#include <memory>

class Cpu {
public:
    Cpu(const std::vector<uint8_t>& code, std::unique_ptr<Mmu> mmu);
    void runCommand();

private:
    std::vector<uint8_t> code;

    uint8_t a = 0; // Accumulator
    uint8_t f = 0; // Status flags

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

    void setHL(uint8_t hi, uint8_t lo);
    uint16_t getHL();
    void decrementHL();
    void setSP(uint8_t hi, uint8_t lo);
    friend std::ostream& operator<<(std::ostream& os, Cpu const& cpu);
};

#endif // CPU_H
