#include "cpu.h"

#include "utils.h"

#include <iostream>

#include "fmt/format.h"

const uint8_t flagZ = 7;
const uint8_t flagN = 6;
const uint8_t flagH = 5;

Cpu::Cpu(const byteCodePtr& code, std::unique_ptr<Mmu> mmu)
    : code(code)
    , mmu(std::move(mmu))
{
}

uint16_t Cpu::getPc() const
{
    return pc;
}

uint16_t Cpu::getHL() const
{
    return static_cast<uint16_t>(h << 8 | l);
}

void Cpu::setHL(uint8_t hi, uint8_t lo)
{
    h = hi;
    l = lo;
}

void Cpu::setHL(uint16_t nn)
{
    h = nn >> 8;
    l = nn & 0xff;
}

void Cpu::decrementHL()
{
    setHL(getHL() - 1);
}

void Cpu::setSP(uint8_t hi, uint8_t lo)
{
    sp = static_cast<uint16_t>(hi << 8) | lo;
}

std::string Cpu::toString() const
{
    std::string flags = f.to_string().substr(0, 4);
    return fmt::format("pc={:04x} a={:02x} c={:02x} f={} hl={:02x}{:02x} sp={:04x}", pc, a, c, flags, h, l, sp);
}

std::ostream& operator<<(std::ostream& os, Cpu const& cpu)
{
    return os << cpu.toString();
}

void Cpu::setFlag(uint8_t flag, bool b)
{
    b ? f.set(flag) : f.reset(flag);
}

bool Cpu::runExtendedCommand()
{
    bool success = true;
    switch (code->at(pc + 1)) {
    case 0x7C:
        // BIT 7,H
        setFlag(flagZ, !isBitSet(7, h));
        setFlag(flagN, 0);
        setFlag(flagH, 1);
        cycles += 8;
        break;
    default:
        std::cerr << fmt::format("Unimplemented opcode: {:02X} {:02X}\n", code->at(pc), code->at(pc + 1));
        success = false;
        break;
    }

    return success;
}

uint8_t Cpu::ld(uint8_t& reg)
{
    // LD reg,n
    reg = code->at(pc + 1);
    cycles += 8;
    return 2;
}

bool Cpu::runCommand()
{
    bool success = true;
    uint8_t bytes = 1;
    switch (code->at(pc)) {
    case 0x0E:
        // LD C,n
        bytes = ld(c);
        break;
    case 0x20:
        // JR NZ,n
        if (f[flagZ]) {
            pc += 2 + static_cast<int8_t>(code->at(pc + 1));
            return success;
        }
        cycles += 8;
        bytes = 2;
        break;
    case 0x21:
        // LD HL,nn
        setHL(code->at(pc + 2), code->at(pc + 1));
        cycles += 12;
        bytes = 3;
        break;
    case 0x31: {
        // LD SP,nn
        setSP(code->at(pc + 2), code->at(pc + 1));
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
    case 0x3E:
        // LD A,n
        bytes = ld(a);
        break;
    case 0xAF:
        // XOR A
        a = a ^ a;
        cycles += 4;
        break;
    case 0xCB:
        bytes = 2;
        success = runExtendedCommand();
        break;
    case 0xE2:
        // LD ($FF00+C),A
        mmu->set(0xFF00 + c, a);
        cycles += 8;
        break;
    default:
        std::cerr << fmt::format("Unimplemented opcode: {:02X}\n", code->at(pc));
        success = false;
        bytes = 0;
        break;
    }

    pc += bytes;
    return success;
}
