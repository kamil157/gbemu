#include "cpu.h"

#include "utils.h"

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "fmt/format.h"
#include "spdlog/spdlog.h"

void run(const byteCodePtr& code)
{
    auto mmu = std::make_unique<Mmu>();
    Cpu cpu{ code, std::move(mmu) };
    uint16_t pc = 0u;
    while (cpu.runCommand()) {
        Instruction instr = disassemble(code, pc);
        std::stringstream bytes;
        for (auto byte : instr.bytes) {
            bytes << fmt::format("{:02X} ", byte);
        }
        spdlog::debug("{:04x} {:<10} {:<6} {:<13} {}", instr.pc, bytes.str(), instr.mnemonic, instr.operands, cpu.toString());
        pc = cpu.getPc();
    }
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
