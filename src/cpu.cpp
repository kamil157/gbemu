#include "cpu.h"

#include "utils.h"

#include <iostream>

#include "fmt/format.h"

using json = nlohmann::json;

const uint8_t flagZ = 7;
const uint8_t flagN = 6;
const uint8_t flagH = 5;
const uint8_t flagC = 4;

Cpu::Cpu(const byteCodePtr& code, std::unique_ptr<Mmu> mmu)
    : code(code)
    , bc(0)
    , de(0)
    , hl(0)
    , mmu(std::move(mmu))
{
}

uint16_t Cpu::getPc() const
{
    return pc;
}

void Cpu::setSP(uint8_t hi, uint8_t lo)
{
    sp = static_cast<uint16_t>(hi << 8) | lo;
}

std::string Cpu::toString() const
{
    std::string flags = f.to_string().substr(0, 4);
    return fmt::format("a={:02x} f={} bc={:04x} de={:04x} hl={:04x} sp={:04x} pc={:04x} ", a, flags, bc, de, hl, sp, pc);
}

std::ostream& operator<<(std::ostream& os, Cpu const& cpu)
{
    return os << cpu.toString();
}

void Cpu::setFlag(uint8_t flag, bool b)
{
    b ? f.set(flag) : f.reset(flag);
}

void Cpu::setFlagsFromJson(json opcode)
{
    auto flags = opcode.at("flags");
    if (flags.at(0) == "0") {
        setFlag(flagZ, 0);
    }
    if (flags.at(0) == "1") {
        setFlag(flagZ, 1);
    }
    if (flags.at(1) == "0") {
        setFlag(flagN, 0);
    }
    if (flags.at(1) == "1") {
        setFlag(flagN, 1);
    }
    if (flags.at(2) == "0") {
        setFlag(flagH, 0);
    }
    if (flags.at(2) == "1") {
        setFlag(flagH, 1);
    }
    if (flags.at(3) == "0") {
        setFlag(flagC, 0);
    }
    if (flags.at(3) == "1") {
        setFlag(flagC, 1);
    }
}

void Cpu::rotateLeft(uint8_t& reg)
{
    setFlag(flagC, reg >> 7);
    reg <<= 1;
    setFlag(flagZ, reg == 0);
}

bool Cpu::runExtendedCommand()
{
    bool success = true;

    switch (byte1()) {
    case 0x11:
        // RL C
        rotateLeft(c);
        break;
    case 0x7C:
        // BIT 7,H
        setFlag(flagZ, !isBitSet(7, h));
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

void Cpu::push(uint16_t nn)
{
    mmu->set(--sp, nn >> 8);
    mmu->set(--sp, nn & 0xFF);
}

bool Cpu::runCommand()
{
    bool success = true;
    uint8_t length = 1;

    if (auto opcode = getOpcode(code, pc)) {
        setFlagsFromJson(*opcode);
        length = opcode->at("length");
        auto jsonCycles = opcode->at("cycles");
        if (jsonCycles.size() == 1) {
            // There can be 2 values here, instructions need to implement that themselves.
            cycles += int(jsonCycles.at(0));
        }
    }

    switch (code->at(pc)) {
    // clang-format off
    case 0x06: b = byte1();                                                        break; // LD B,n
    case 0x0C: c++;                                                                break; // INC C
    case 0x0E: c = byte1();                                                        break; // LD C,n
    case 0x11: d = byte2(); e = byte1();                                           break; // LD DE,nn
    case 0x17: rotateLeft(a);                                                      break; // RLA
    case 0x1A: a = mmu->get(de);                                                   break; // LD A,(DE)
    case 0x20: if (f[flagZ]) pc += static_cast<int8_t>(byte1());                   break; // JR NZ,n
    case 0x21: h = byte2(); l = byte1();                                           break; // LD HL,nn
    case 0x31: setSP(byte2(), byte1());                                            break; // LD SP,nn
    case 0x32: mmu->set(hl--, a);                                                  break; // LDD (HL),A
    case 0x3E: a = byte1();                                                        break; // LD A,n
    case 0x4F: c = a;                                                              break; // LD C,A
    case 0x77: mmu->set(hl, a);                                                    break; // LD (HL),A
    case 0xAF: a = a ^ a;                                                          break; // XOR A
    case 0xC5: push(bc);                                                           break; // PUSH BC
    case 0xCB: success = runExtendedCommand();                                     break;
    case 0xCD: push(pc + 1); pc = byte1();                                   return true; // CALL nn
    case 0xE0: mmu->set(0xFF00 + byte1(), a);                                      break; // LDH ($FF00+n),A
    case 0xE2: mmu->set(0xFF00 + c, a);                                            break; // LD ($FF00+C),A
    // clang-format on
    default:
        std::cerr << fmt::format("Unimplemented opcode: {:02X}\n", code->at(pc));
        return false;
    }

    pc += length;
    return success;
}
