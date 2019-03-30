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

Instruction disassembleExtendedCommand(const byteCodePtr& code, uint16_t& pc)
{
    Instruction instr;
    instr.pc = pc;
    switch (code->at(pc + 1)) {
    case 0x7C:
        // Test bit 7 in register H.
        instr.mnemonic = "BIT";
        instr.operands = "7,H";
        break;
    }

    return instr;
}

// Disassemble 8080 opcodes into assembly language
Instruction disassemble(const byteCodePtr& code, uint16_t pc)
{
    Instruction instr;
    instr.pc = pc;
    auto opbytes = 1u;
    switch (code->at(pc)) {
    case 0x00:
        // No operation.
        instr.mnemonic = "NOP";
        break;
    case 0x01:
        // Put value nn into BC.
        instr.mnemonic = "LD";
        instr.operands = fmt::format("BC,{:02x}{:02x}", code->at(pc + 2), code->at(pc + 1));
        break;
    case 0x06:
        // Put value n into B.
        instr.mnemonic = "LD";
        instr.operands = fmt::format("B,{:02x}", code->at(pc + 1));
        opbytes = 2;
        break;
    case 0x0E:
        // Put value n into C.
        instr.mnemonic = "LD";
        instr.operands = fmt::format("C,{:02x}", code->at(pc + 1));
        opbytes = 2;
        break;
    case 0x20:
        // JR NZ,n
        instr.mnemonic = "JR";
        instr.operands = fmt::format("NZ,${:02x}", code->at(pc + 1));
        opbytes = 2;
        break;
    case 0x21:
        // Put value nn into HL.
        instr.mnemonic = "LD";
        instr.operands = fmt::format("HL,${:02x}{:02x}", code->at(pc + 2), code->at(pc + 1));
        opbytes = 3;
        break;
    case 0x2A:
        // Put value at address HL into A. Increment HL.
        // Same as: LD A,(HL) - INC HL
        instr.mnemonic = "LDI";
        instr.operands = "A,(HL)";
        break;
    case 0x31:
        // LD SP,nn
        instr.mnemonic = "LD";
        instr.operands = fmt::format("SP,${:02x}{:02x}", code->at(pc + 2), code->at(pc + 1));
        opbytes = 3;
        break;
    case 0x32:
        // Put A into memory address HL. Decrement HL.
        // Same as: LD (HL),A - DEC HL
        instr.mnemonic = "LDD";
        instr.operands = "(HL),A";
        break;
    case 0x3A:
        // Put value at address HL into A. Decrement HL.
        // Same as: LD A,(HL) - DEC HL
        instr.mnemonic = "LDD";
        instr.operands = "A,(HL)";
        break;
    case 0x3E:
        // Put value n into A.
        instr.mnemonic = "LD";
        instr.operands = fmt::format("A,{:02x}", code->at(pc + 1));
        opbytes = 2;
        break;
    case 0x47:
        // Put value A into B.
        instr.mnemonic = "LD";
        instr.operands = "B,A";
        break;
    case 0x78:
        // Put value B into A.
        instr.mnemonic = "LD";
        instr.operands = "A,B";
        break;
    case 0xAF:
        // Logical exclusive OR n with register A, result in A.
        instr.mnemonic = "XOR";
        instr.operands = "A";
        break;
    case 0xCB:
        instr = disassembleExtendedCommand(code, pc);
        opbytes = 2;
        break;
    case 0xE0:
        // Put A into memory address $FF00+n.
        instr.mnemonic = "LD";
        instr.operands = fmt::format("($FF00+{:02x}),A", code->at(pc + 1));
        opbytes = 2;
        break;
    case 0xF2:
        // Put value at address $FF00 + register C into A.
        // Same as: LD A,($FF00+C)
        instr.mnemonic = "LD";
        instr.operands = "A,(C)";
        break;
    }

    for (uint8_t i = 0; i < opbytes; ++i) {
        instr.bytes.push_back(code->at(pc + i));
    }

    return instr;
}
