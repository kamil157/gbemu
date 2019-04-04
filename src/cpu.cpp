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

    uint8_t byte = read();
    switch (byte) {
    case 0x11:
        // RL C
        rotateLeft(c);
        break;
    case 0x7C:
        // BIT 7,H
        setFlag(flagZ, !isBitSet(7, h));
        break;
    default:
        std::cerr << fmt::format("Unimplemented opcode: CB {:02X}\n", byte);
        return false;
    }

    return success;
}

uint8_t Cpu::read()
{
    return code->at(pc++);
}

uint16_t Cpu::read16()
{
    uint8_t lo = read();
    uint8_t hi = read();
    return concatBytes(lo, hi);
}

void Cpu::push(uint16_t nn)
{
    mmu->set(--sp, nn >> 8);
    mmu->set(--sp, nn & 0xFF);
}

// JR flag,n
void Cpu::relativeJump(uint8_t flag)
{
    uint8_t offset = read();
    if (f[flag]) {
        pc += static_cast<int8_t>(offset);
    }
}

// CALL nn
void Cpu::call()
{
    uint8_t lo = read();
    uint8_t hi = read();
    push(pc);
    pc = concatBytes(lo, hi);
}

bool Cpu::runCommand()
{
    bool success = true;

    if (auto opcode = getOpcode(code, pc)) {
        setFlagsFromJson(*opcode);
        auto jsonCycles = opcode->at("cycles");
        if (jsonCycles.size() == 1) {
            // There can be 2 values here, instructions need to implement that themselves.
            cycles += int(jsonCycles.at(0));
        }
    }

    uint8_t byte = read();
    switch (byte) {
    // clang-format off
    case 0x06: b = read();                                                         break; // LD B,n
    case 0x0C: c++;                                                                break; // INC C
    case 0x0E: c = read();                                                         break; // LD C,n
    case 0x11: de = read16();                                                      break; // LD DE,nn
    case 0x17: rotateLeft(a);                                                      break; // RLA
    case 0x1A: a = mmu->get(de);                                                   break; // LD A,(DE)
    case 0x20: relativeJump(flagZ);                                                break; // JR NZ,n
    case 0x21: hl = read16();                                                      break; // LD HL,nn
    case 0x31: sp = read16();                                                      break; // LD SP,nn
    case 0x32: mmu->set(hl--, a);                                                  break; // LDD (HL),A
    case 0x3E: a = read();                                                         break; // LD A,n
    case 0x4F: c = a;                                                              break; // LD C,A
    case 0x77: mmu->set(hl, a);                                                    break; // LD (HL),A
    case 0xAF: a = a ^ a;                                                          break; // XOR A
    case 0xC5: push(bc);                                                           break; // PUSH BC
    case 0xCB: success = runExtendedCommand();                                     break;
    case 0xCD: call();                                                             break; // CALL nn
    case 0xE0: mmu->set(0xFF00 + read(), a);                                       break; // LDH ($FF00+n),A
    case 0xE2: mmu->set(0xFF00 + c, a);                                            break; // LD ($FF00+C),A
    // clang-format on
    default:
        std::cerr << fmt::format("Unimplemented opcode: {:02X}\n", byte);
        return false;
    }

    return success;
}
