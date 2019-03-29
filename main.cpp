#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "fmt/format.h"

// Disassemble 8080 opcodes into assembly language
std::string disassemble(const std::vector<uint8_t>& code, uint& pc)
{
    auto opbytes = 1u;
    std::stringstream ss;
    ss << fmt::format("{:04x} ", pc);
    switch (code[pc]) {
    case 0x00:
        // No operation.
        ss << "NOP";
        break;
    case 0x01:
        // Put value nn into BC.
        ss << fmt::format("LD     BC,{:02x}{:02x}", code[pc + 2], code[pc + 1]);
        break;
    case 0x06:
        // Put value B into memory at n.
        ss << fmt::format("LD     B,{:02x}", code[pc + 1]);
        opbytes = 2;
        break;
    case 0x2A:
        // Put value at address HL into A. Increment HL.
        // Same as: LD A,(HL) - INC HL
        ss << "LDI    A,(HL)";
        break;
    case 0x3A:
        // Put value at address HL into A. Decrement HL.
        // Same as: LD A,(HL) - DEC HL
        ss << "LDD    A,(HL)";
        break;
    case 0x47:
        // Put value A into B.
        ss << "LD     B,A";
        break;
    case 0x78:
        // Put value B into A.
        ss << "LD     A,B";
        break;
    case 0xE0:
        // Put A into memory address $FF00+n.
        ss << fmt::format("LD     ($FF00+{:02x}),A", code[pc + 1]);
        break;
    case 0xF2:
        // Put value at address $FF00 + register C into A.
        // Same as: LD A,($FF00+C)
        ss << "LD     A,(C)";
        break;
    }

    pc += opbytes;
    return ss.str();
}

std::vector<uint8_t> readFile(const std::string& path)
{
    std::ifstream input(path, std::ios::binary);
    if (input.fail()) {
        throw std::runtime_error("Couldn't open file: " + path);
    }

    // copies all data into buffer
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(input), {});
    return buffer;
}

int main(int argc, char** argv)
{
    try {
        if (argc < 2) {
            throw std::runtime_error("Please provide rom name.");
        }
        auto rom = readFile(argv[1]);
        auto pc = 0u;
        while (pc < rom.size()) {
            std::cout << disassemble(rom, pc) << std::endl;
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
