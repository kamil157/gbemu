#include "cpu.h"

#include "utils.h"

#include <exception>
#include <iostream>
#include <stdexcept>

#include "fmt/format.h"

Cpu::Cpu(const std::vector<uint8_t>& code, std::unique_ptr<Mmu> mmu)
    : code(code)
    , mmu(std::move(mmu))
{
}

uint16_t Cpu::getHL()
{
    return static_cast<uint16_t>(h << 8 | l);
}

void Cpu::setHL(uint8_t hi, uint8_t lo)
{
    h = hi;
    l = lo;
}

void Cpu::decrementHL()
{
    if (l == 0) {
        --h;
    }
    --l;
}

void Cpu::setSP(uint8_t hi, uint8_t lo)
{
    sp = static_cast<uint16_t>(hi << 8) | lo;
}

std::ostream& operator<<(std::ostream& os, Cpu const& cpu)
{
    return os << fmt::format("pc={:04x} a={:02x} hl={:02x}{:02x} sp={:04x}", cpu.pc, cpu.a, cpu.h, cpu.l, cpu.sp);
}

void Cpu::runCommand()
{
    uint8_t bytes = 1;
    switch (code.at(pc)) {
    case 0x21:
        // LD HL,nn
        setHL(code.at(pc + 2), code.at(pc + 1));
        cycles += 12;
        bytes = 3;
        break;
    case 0x31: {
        // LD SP,nn
        setSP(code.at(pc + 2), code.at(pc + 1));
        cycles += 12;
        bytes = 3;
        break;
    }
    case 0x32: {
        // LDD (HL),A
        mmu->set(getHL(), a);
        decrementHL();
        cycles += 8;
        break;
    }
    case 0xAF:
        // XOR A
        a = a ^ a;
        cycles += 4;
        break;
    }
    pc += bytes;
}

void run(const std::vector<uint8_t>& code)
{
    auto mmu = std::make_unique<Mmu>();
    Cpu cpu{ code, std::move(mmu) };
    for (int i = 0; i < 4; ++i) {
        cpu.runCommand();
        std::cout << cpu << std::endl;
    }
}

int main(int argc, char** argv)
{
    try {
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
