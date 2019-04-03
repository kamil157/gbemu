#include "cpu.h"

#include "utils.h"

#include <iostream>

#include "fmt/format.h"

const uint8_t flagZ = 7;
const uint8_t flagN = 6;
const uint8_t flagH = 5;

Cpu::Cpu(const byteCodePtr& code, std::unique_ptr<Mmu> mmu)
    : code(code)
    , hl(0)
    , mmu(std::move(mmu))
{
}

uint16_t Cpu::getPc() const
{
    return pc;
}

uint16_t Cpu::getDE() const
{
    return static_cast<uint16_t>(d << 8 | e);
}

void Cpu::setDE(uint8_t hi, uint8_t lo)
{
    d = hi;
    e = lo;
}

void Cpu::setSP(uint8_t hi, uint8_t lo)
{
    sp = static_cast<uint16_t>(hi << 8) | lo;
}

std::string Cpu::toString() const
{
    std::string flags = f.to_string().substr(0, 4);
    return fmt::format("a={:02x} f={} bc={:02x}{:02x} de={:02x}{:02x} hl={:02x}{:02x} sp={:04x} pc={:04x} ", a, flags, b, c, d, e, h, l, sp, pc);
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
    switch (byte1()) {
    case 0x7C:
        // BIT 7,H
        setFlag(flagZ, !isBitSet(7, h));
        setFlag(flagN, 0);
        setFlag(flagH, 1);
        break;
    default:
        std::cerr << fmt::format("Unimplemented opcode: {:02X} {:02X}\n", code->at(pc), byte1());
        return false;
    }

    return success;
}

uint8_t Cpu::byte1()
{
    return code->at(pc + 1);
}

uint8_t Cpu::byte2()
{
    return code->at(pc + 2);
}

bool Cpu::runCommand()
{
    bool success = true;
    uint8_t length = 1;

    if (auto opcode = getOpcode(code, pc)) {
        length = opcode->at("length");
        auto jsonCycles = opcode->at("cycles");
        if (jsonCycles.size() == 1) {
            // There can be 2 values here, instructions need to implement that themselves.
            cycles += int(jsonCycles.at(0));
        }
    }
    switch (code->at(pc)) {
    // clang-format off
    case 0x0C: c++;                                              break; // INC C
    case 0x0E: c = byte1();                                      break; // LD C,n
    case 0x11: setDE(byte2(), byte1());                          break; // LD DE,nn
    case 0x1A: a = mmu->get(getDE());                            break; // LD A,(DE)
    case 0x20: if (f[flagZ]) pc += static_cast<int8_t>(byte1()); break; // JR NZ,n
    case 0x21: h = byte2(); l = byte1();                         break; // LD HL,nn
    case 0x31: setSP(byte2(), byte1());                          break; // LD SP,nn
    case 0x32: mmu->set(hl--, a);                                break; // LDD (HL),A
    case 0x3E: a = byte1();                                      break; // LD A,n
    case 0x77: mmu->set(hl, a);                                  break; // LD (HL),A
    case 0xAF: a = a ^ a;                                        break; // XOR A
    case 0xCB: success = runExtendedCommand();                   break;
    case 0xE0: mmu->set(0xFF00 + byte1(), a);                    break; // LDH ($FF00+n),A
    case 0xE2: mmu->set(0xFF00 + c, a);                          break; // LD ($FF00+C),A
    // clang-format on
    default:
        std::cerr << fmt::format("Unimplemented opcode: {:02X}\n", code->at(pc));
        return false;
    }

    pc += length;
    return success;
}
