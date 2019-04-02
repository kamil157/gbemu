#include "cpu.h"

#include "utils.h"

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "spdlog/spdlog.h"

void run(const byteCodePtr& code)
{
    auto mmu = std::make_unique<Mmu>();
    Cpu cpu{ code, std::move(mmu) };
    uint16_t pc = 0u;
    while (cpu.runCommand()) {
        Instruction instr = disassemble(code, pc);
        std::cout << fmt::format("{:04x} {:<10} {:<6} {:<13} {}\n", instr.pc, instr.bytesToString(), instr.mnemonic, instr.operandsToString(), cpu.toString());
        pc = cpu.getPc();
    }
    Instruction instr = disassemble(code, pc);
    std::cout << fmt::format("{:04x} {:<10} {:<6} {:<13}\n", instr.pc, instr.bytesToString(), instr.mnemonic, instr.operandsToString());
}

int main(int argc, char** argv)
{
    try {
        spdlog::set_level(spdlog::level::debug);
        spdlog::set_pattern("%v");
        if (argc < 2) {
            throw std::runtime_error("Please provide rom name.");
        }
        auto rom = readFile(argv[1]);
        run(rom);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
