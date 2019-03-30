#include "utils.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "fmt/format.h"

std::string disassembleExtendedCommand(const byteCodePtr& code, uint& pc)
{
    switch (code->at(pc + 1)) {
    case 0x7C:
        // Test bit 7 in register H.
        return "BIT    7,H";
    }

    return "";
}

// Disassemble 8080 opcodes into assembly language
std::string disassemble(const byteCodePtr& code, uint& pc)
{
    auto opbytes = 1u;
    std::stringstream ss;
    ss << fmt::format("{:04x} ", pc);
    switch (code->at(pc)) {
    case 0x00:
        // No operation.
        ss << "NOP";
        break;
    case 0x01:
        // Put value nn into BC.
        ss << fmt::format("LD     BC,{:02x}{:02x}", code->at(pc + 2), code->at(pc + 1));
        break;
    case 0x06:
        // Put value B into memory at n.
        ss << fmt::format("LD     B,{:02x}", code->at(pc + 1));
        opbytes = 2;
        break;
    case 0x21:
        // Put value nn into HL.
        ss << fmt::format("LD     HL,${:02x}{:02x}", code->at(pc + 2), code->at(pc + 1));
        opbytes = 3;
        break;
    case 0x2A:
        // Put value at address HL into A. Increment HL.
        // Same as: LD A,(HL) - INC HL
        ss << "LDI    A,(HL)";
        break;
    case 0x31:
        // LD SP,nn
        ss << fmt::format("LD     SP,${:02x}{:02x}", code->at(pc + 2), code->at(pc + 1));
        opbytes = 3;
        break;
    case 0x32:
        // Put A into memory address HL. Decrement HL.
        // Same as: LD (HL),A - DEC HL
        ss << "LDD    (HL),A";
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
    case 0xAF:
        // Logical exclusive OR n with register A, result in A.
        ss << "XOR    A";
        break;
    case 0xCB:
        ss << disassembleExtendedCommand(code, pc);
        opbytes = 2;
        break;
    case 0xE0:
        // Put A into memory address $FF00+n.
        ss << fmt::format("LD     ($FF00+{:02x}),A", code->at(pc + 1));
        opbytes = 2;
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

int main(int argc, char** argv)
{
    try {
        if (argc < 2) {
            throw std::runtime_error("Please provide rom name.");
        }
        auto rom = readFile(argv[1]);
        auto pc = 0u;
        while (pc < rom->size()) {
            std::cout << disassemble(rom, pc) << std::endl;
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
