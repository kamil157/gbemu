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

std::optional<json> getOpcode(const byteCodePtr& code, uint16_t pc)
{
    bool isPrefixed = code->at(pc) == 0xCB;
    std::string commandType = isPrefixed ? "cbprefixed" : "unprefixed";
    uint8_t opcode = isPrefixed ? code->at(pc + 1) : code->at(pc);
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

// Disassemble LR35902 opcodes into assembly language
Instruction disassemble(const byteCodePtr& code, uint16_t pc)
{
    Instruction instr;
    instr.pc = pc;
    auto opbytes = 1u;
    if (auto opcode = getOpcode(code, pc)) {
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
        instr.bytes.push_back(code->at(pc + i));
    }

    if (instr.operand2 == "d8" || instr.operand2 == "r8") {
        instr.operand2 = fmt::format("${:02x}", code->at(pc + 1));
    } else if (instr.operand2 == "d16") {
        instr.operand2 = fmt::format("${:02x}{:02x}", code->at(pc + 2), code->at(pc + 1));
    }

    switch (code->at(pc)) {
    case 0xE0:
        // Put A into memory address $FF00+n.
        instr.operand1 = fmt::format("($FF00+{:02x})", code->at(pc + 1));
        break;
    case 0xE2:
        // Put A into address $FF00 + register C.
        instr.operand1 = "($FF00+C)";
        break;
    }

    return instr;
}
