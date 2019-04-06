#include "cpu.h"

#include "utils.h"

#include <exception>
#include <stdexcept>

#include "spdlog/spdlog.h"

void run(const byteCodePtr& code)
{
    auto mmu = std::make_unique<Mmu>();
    Cpu cpu{ code, std::move(mmu) };
    uint16_t pc = 0u;
    while (cpu.runCommand()) {
        Instruction instr = disassemble(code, pc);
        spdlog::trace("{:04x} {:<10} {:<6} {:<13} {}", instr.pc, instr.bytesToString(), instr.mnemonic, instr.operandsToString(), cpu.toString());
        pc = cpu.getPc();
    }
    Instruction instr = disassemble(code, pc);
    spdlog::trace("{:04x} {:<10} {:<6} {:<13}", instr.pc, instr.bytesToString(), instr.mnemonic, instr.operandsToString());
}

int main(int argc, char** argv)
{
    try {
        spdlog::set_level(spdlog::level::trace);
        spdlog::set_pattern("%v");
        if (argc < 2) {
            throw std::runtime_error("Please provide rom name.");
        }
        auto rom = readFile(argv[1]);
        run(rom);
        return 0;
    } catch (const std::exception& e) {
        spdlog::error("{}\n", e.what());
        return 1;
    }
}
