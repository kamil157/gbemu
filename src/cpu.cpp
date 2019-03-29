#include "cpu.h"

#include "utils.h"

#include <exception>
#include <iostream>
#include <stdexcept>

Cpu::Cpu(const std::vector<uint8_t>& code)
    : code(code)
{
}

void Cpu::runCommand()
{
    uint8_t bytes = 1;
    switch (code[pc]) {
    case 0x31:
        // LD SP,nn
        uint8_t lo = code[pc + 1];
        uint8_t hi = code[pc + 2];
        sp = static_cast<uint16_t>(hi << 8u) | lo;
        cycles += 12;
        bytes = 3;
        break;
    }
    pc += bytes;
}

void run(const std::vector<uint8_t>& code)
{
    Cpu cpu{ code };
    cpu.runCommand();
}

int main(int argc, char** argv)
{
    try {
        if (argc < 2) {
            throw std::runtime_error("Please provide rom name.");
        }
        auto rom = readFile(argv[1]);
        run(rom);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
