#include "utils.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "fmt/format.h"

using json = nlohmann::json;

static json opcodes = readOpcodes();

std::string Instruction::bytesToString()
{
    std::stringstream bytes;
    for (auto byte : this->bytes) {
        bytes << fmt::format("{:02X} ", byte);
    }
    return bytes.str();
}

std::string Instruction::operandsToString()
{
    std::stringstream operands;
    if (operand1) {
        operands << *operand1;
    }
    if (operand2) {
        operands << "," << *operand2;
    }
    return operands.str();
}

std::vector<uint8_t> readFile(const std::string& path)
{
    std::ifstream input(path, std::ios::binary);
    if (input.fail()) {
        throw std::runtime_error("Couldn't open file: " + path);
    }

    // Copies all data into buffer
    return std::vector<uint8_t>{ std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>() };
}

bool isHalfCarryAddition(uint8_t n, uint8_t m)
{
    return ((n & 0xf) + (m & 0xf)) > 0xf;
}

bool isHalfCarrySubtraction(int8_t n, int8_t m)
{
    return (n & 0xf) - (m & 0xf) < 0;
}

bool isHalfCarryAddition16(uint16_t n, uint16_t m)
{
    return ((n & 0xfff) + (m & 0xfff)) > 0xfff;
}

bool isBitSet(uint8_t n, uint8_t byte)
{
    if (n > 7) {
        throw std::invalid_argument("n must be between 0 and 7.");
    }
    return (byte >> n) & 1;
}

uint16_t concatBytes(uint8_t lo, uint8_t hi)
{
    return static_cast<uint16_t>(hi << 8) | lo;
}

std::optional<json> getOpcodeData(uint8_t unprefixedOpcode, uint8_t prefixedOpcode)
{
    bool isPrefixed = unprefixedOpcode == 0xCB;
    std::string commandType = isPrefixed ? "cbprefixed" : "unprefixed";
    uint8_t opcode = isPrefixed ? prefixedOpcode : unprefixedOpcode;
    std::string opcodeHex = fmt::format("{:#02x}", opcode);
    if (!opcodes.at(commandType).contains(opcodeHex)) {
        return {}; // Unimplemented
    }
    return opcodes.at(commandType).at(opcodeHex);
}

json readOpcodes()
{
    std::string path = "../gbemu/lib/opcodes/opcodes.json";
    std::ifstream input(path);
    if (input.fail()) {
        throw std::runtime_error("Couldn't open file: " + path);
    }
    json opcodes;
    input >> opcodes;
    return opcodes;
}

std::optional<std::string> readOperandValue(const std::vector<uint8_t>& memory, uint16_t pc, const std::optional<std::string>& operand)
{
    if (operand == "d8" || operand == "r8") {
        return fmt::format("${:02x}", memory.at(pc + 1));
    } else if (operand == "d16" || operand == "a16") {
        return fmt::format("${:02x}{:02x}", memory.at(pc + 2), memory.at(pc + 1));
    } else if (operand == "(a16)") {
        return fmt::format("$({:02x}{:02x})", memory.at(pc + 2), memory.at(pc + 1));
    }
    return operand;
}

Instruction disassemble(const std::vector<uint8_t>& memory, uint16_t pc)
{
    Instruction instr;
    instr.pc = pc;
    auto opbytes = 1u;
    if (auto opcode = getOpcodeData(memory.at(pc), memory.size() > pc + 1 ? memory.at(pc + 1) : 0)) {
        instr.mnemonic = opcode->at("mnemonic");
        opbytes = opcode->at("length");
        if (opcode->contains("operand1")) {
            instr.operand1 = opcode->at("operand1");
        }
        if (opcode->contains("operand2")) {
            instr.operand2 = opcode->at("operand2");
        }
    }
    for (uint8_t i = 0; i < opbytes; ++i) {
        instr.bytes.push_back(memory.at(pc + i));
    }

    instr.operand1 = readOperandValue(memory, pc, instr.operand1);
    instr.operand2 = readOperandValue(memory, pc, instr.operand2);

    switch (memory.at(pc)) {
    case 0xE0:
        // Put A into memory address $FF00+n.
        instr.operand1 = fmt::format("($FF{:02x})", memory.at(pc + 1));
        break;
    case 0xE2:
        // Put A into address $FF00 + register C.
        instr.operand1 = "($FF00+C)";
        break;
    case 0xF0:
        instr.operand2 = fmt::format("($FF{:02x})", memory.at(pc + 1));
        break;

    // Relative jumps - print absolute address instead of relative.
    case 0x18:
        instr.operand1 = fmt::format("${:04x}", pc + 1 + static_cast<int8_t>(memory.at(pc + 1)));
        break;
    case 0x20:
    case 0x28:
    case 0x30:
    case 0x38:
        instr.operand2 = fmt::format("${:04x}", pc + 2 + static_cast<int8_t>(memory.at(pc + 1)));
        break;
    }

    return instr;
}
