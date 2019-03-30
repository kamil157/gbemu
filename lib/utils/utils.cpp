#include "utils.h"

#include <fstream>
#include <sstream>

#include "fmt/format.h"

byteCodePtr readFile(const std::string& path)
{
    std::ifstream input(path, std::ios::binary);
    if (input.fail()) {
        throw std::runtime_error("Couldn't open file: " + path);
    }

    // Copies all data into buffer
    return std::make_shared<std::vector<uint8_t>>(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

bool isBitSet(uint8_t byte, uint8_t n)
{
    return (byte >> n) & 1;
}

std::string disassembleExtendedCommand(const byteCodePtr& code, uint16_t& pc)
{
    switch (code->at(pc + 1)) {
    case 0x7C:
        // Test bit 7 in register H.
        return "BIT    7,H";
    }

    return "";
}

// Disassemble 8080 opcodes into assembly language
std::string disassemble(const byteCodePtr& code, uint16_t& pc)
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
        // Put value n into B.
        ss << fmt::format("LD     B,{:02x}", code->at(pc + 1));
        opbytes = 2;
        break;
    case 0x0E:
        // Put value n into C.
        ss << fmt::format("LD     C,{:02x}", code->at(pc + 1));
        opbytes = 2;
        break;
    case 0x20:
        // JR NZ,n
        ss << fmt::format("JR     NZ,${:02x}", code->at(pc + 1));
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
    case 0x3E:
        // Put value n into A.
        ss << fmt::format("LD     A,{:02x}", code->at(pc + 1));
        opbytes = 2;
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
