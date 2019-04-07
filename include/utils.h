#ifndef UTILS_H
#define UTILS_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <json.hpp>

// Read whole file into a buffer and return it.
std::vector<uint8_t> readFile(const std::string& path);

// Return true if adding n + m results in carry from low nibble to high (bit 3 to 4).
bool isHalfCarryAddition(uint8_t n, uint8_t m);

// Return true if subtracting n - m does not result in borrowing from high nibble to low (bit 4 to 3).
bool isHalfCarrySubtraction(int8_t n, int8_t m);

// Return true if adding n + m results in carry from low nibble to high in high byte (bit 11 to 12).
bool isHalfCarryAddition16(uint16_t n, uint16_t m);

// Test if bit n of byte is set.
bool isBitSet(uint8_t byte, uint8_t n);

// Concatenate 2 bytes into 16 bit int.
uint16_t concatBytes(uint8_t lo, uint8_t hi);

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

// Get opcode data from json.
std::optional<nlohmann::json> getOpcodeData(uint8_t unprefixedOpcode, uint8_t prefixedOpcode);

// Disassemble LR35902 opcodes into assembly language.
Instruction disassemble(const std::vector<uint8_t>& code, uint16_t pc);

#endif // UTILS_H
