#include "cpu.h"

#include "utils.h"

#include <exception>
#include <stdexcept>

#include "spdlog/spdlog.h"

void run(const std::string& romFilename)
{
    auto mmu = std::make_unique<Mmu>();
    auto bootstrapRom = readFile("../gbemu/res/bootstrap.bin");
    mmu->load(0, bootstrapRom);
    auto cartridgeRom = readFile(romFilename);
    mmu->load(0x100, cartridgeRom);
    Cpu cpu{ std::move(mmu) };
    uint16_t pc = 0u;
    bootstrapRom->insert(bootstrapRom->end(), cartridgeRom->begin(), cartridgeRom->end());
    while (cpu.runCommand()) {
        Instruction instr = disassemble(bootstrapRom, pc);
        spdlog::trace("{:04x} {:<10} {:<6} {:<13} {}", instr.pc, instr.bytesToString(), instr.mnemonic, instr.operandsToString(), cpu.toString());
        pc = cpu.getPc();
    }
    Instruction instr = disassemble(bootstrapRom, pc);
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
        auto romFilename = argv[1];
        run(romFilename);
        return 0;
    } catch (const std::exception& e) {
        spdlog::error(e.what());
        return 1;
    }
}
