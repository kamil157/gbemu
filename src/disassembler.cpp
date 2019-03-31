#include "utils.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "fmt/format.h"

int main(int argc, char** argv)
{
    try {
        if (argc < 2) {
            throw std::runtime_error("Please provide rom name.");
        }
        auto rom = readFile(argv[1]);
        uint16_t pc = 0u;
        while (pc < rom->size()) {
            Instruction instr = disassemble(rom, pc);
            std::cout << fmt::format("{:<6} {:<15} ; {:04x} ; {}", instr.mnemonic, instr.operandsToString(), instr.pc, instr.bytesToString()) << std::endl;
            pc += instr.bytes.size();
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
