#ifndef UTILS_H
#define UTILS_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using byteCodePtr = std::shared_ptr<std::vector<uint8_t>>;

// Read whole file into a buffer and return it.
byteCodePtr readFile(const std::string& path);

// Test if bit n of byte is set.
bool isBitSet(uint8_t byte, uint8_t n);

struct Instruction {
    uint16_t pc;
    std::string mnemonic;
    std::optional<std::string> operand1;
    std::optional<std::string> operand2;
    std::vector<uint8_t> bytes;

    std::string bytesToString();
    std::string operandsToString();
};

nlohmann::json readOpcodes();

// Disassemble 8080 opcodes into assembly language.
Instruction disassemble(const byteCodePtr& code, uint16_t pc);

#endif // UTILS_H
