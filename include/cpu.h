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

    // Set flag in register f to b.
    void setFlag(uint8_t flag, bool b);
    void setHL(uint8_t hi, uint8_t lo);
    void setHL(uint16_t nn);
    uint16_t getHL() const;
    void decrementHL();
    void setSP(uint8_t hi, uint8_t lo);
    bool runExtendedCommand();
    uint8_t byte1();
    uint8_t byte2();
};

#endif // CPU_H
