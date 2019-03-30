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
            std::cout << disassemble(rom, pc) << std::endl;
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
