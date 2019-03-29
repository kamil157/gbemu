#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <vector>

class Cpu {
public:
    Cpu(const std::vector<uint8_t>& code);
    void runCommand();

private:
    std::vector<uint8_t> code;

    uint8_t a = 0;
    uint8_t f = 0;
    uint8_t b = 0;
    uint8_t c = 0;
    uint8_t d = 0;
    uint8_t e = 0;
    uint8_t h = 0;
    uint8_t l = 0;
    uint16_t sp = 0; // Stack Pointer
    uint16_t pc = 0; // Program Counter

    bool flagZ = false; // Zero flag
    bool flagN = false; // Subtract Flag
    bool flagH = false; // Half Carry Flag
    bool flagC = false; // Carry Flag

    int cycles = 0;
};

#endif // CPU_H
