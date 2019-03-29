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
        ss << "NOP";
        break;
    case 0x01:
        ss << fmt::format("LXI    B,#${:02x}{:02x}", code[pc + 2], code[pc + 1]);
        opbytes = 3;
        break;
    case 0x02:
        ss << "STAX   B";
        break;
    case 0x03:
        ss << "INX    B";
        break;
    case 0x04:
        ss << "INR    B";
        break;
    case 0x05:
        ss << "DCR    B";
        break;
    case 0x06:
        ss << fmt::format("MVI    B,#${:02x}", code[pc + 1]);
        opbytes = 2;
        break;
    case 0x07:
        ss << "RLC";
        break;
    case 0x08:
        ss << "NOP";
        break;
    /* ........ */
    case 0x3e:
        ss << fmt::format("MVI    A,#${:02x}", code[pc + 1]);
        opbytes = 2;
        break;
    /* ........ */
    case 0xc3:
        ss << fmt::format("JMP    ${:02x}{:02x}", code[pc + 2], code[pc + 1]);
        opbytes = 3;
        break;
        /* ........ */
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
