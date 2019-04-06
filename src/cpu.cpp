#include "cpu.h"

#include "utils.h"

#include <iostream>

#include "fmt/format.h"
#include "spdlog/spdlog.h"

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
    uint8_t opcode = read();
    switch (opcode) {
    case 0x11:
        // RL C
        rotateLeft(c);
        break;
    case 0x7C:
        // BIT 7,H
        setFlag(flagZ, !isBitSet(7, h));
        break;
    default:
        spdlog::error("Unimplemented opcode: CB {:02X}", opcode);
        return false;
    }

    return true;
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

uint16_t Cpu::pop()
{
    uint8_t lo = mmu->get(sp++);
    uint8_t hi = mmu->get(sp++);
    return concatBytes(lo, hi);
}

void Cpu::push(uint16_t nn)
{
    mmu->set(--sp, nn >> 8);
    mmu->set(--sp, nn & 0xFF);
}

// JR flag,n
void Cpu::relativeJump(bool condition)
{
    uint8_t offset = read();
    if (condition) {
        pc += static_cast<int8_t>(offset);
    }
}

// CALL nn
void Cpu::call()
{
    uint16_t target = read16();
    push(pc);
    pc = target;
}

void Cpu::dec(uint8_t& reg)
{
    --reg;
    setFlag(flagH, isHalfCarrySubtraction(static_cast<int8_t>(reg), -1));
    setFlag(flagZ, reg == 0);
}

void Cpu::inc(uint8_t& reg)
{
    ++reg;
    setFlag(flagH, isHalfCarryAddition(reg, 1));
    setFlag(flagZ, reg == 0);
}

void Cpu::xorA(uint8_t reg)
{
    a ^= reg;
    setFlag(flagZ, a == 0);
}

void Cpu::cp(uint8_t n)
{
    setFlag(flagZ, a == n);
    setFlag(flagH, isHalfCarrySubtraction(static_cast<int8_t>(a), static_cast<int8_t>(n)));
    setFlag(flagC, a < n);
}

void Cpu::sub(uint8_t n)
{
    cp(n);
    a -= n;
}

bool Cpu::runCommand()
{
    bool success = true;

    if (auto opcodeData = getOpcodeData(code, pc)) {
        setFlagsFromJson(*opcodeData);
        auto jsonCycles = opcodeData->at("cycles");
        if (jsonCycles.size() == 1) {
            // There can be 2 values here, instructions need to implement that themselves.
            cycles += int(jsonCycles.at(0));
        }
    }

    uint8_t opcode = read();
    switch (opcode) {
    // clang-format off
    case 0x04: inc(b);                                                             break; // INC B
    case 0x05: dec(b);                                                             break; // DEC B
    case 0x06: b = read();                                                         break; // LD B,n
    case 0x0C: inc(c);                                                             break; // INC C
    case 0x0D: dec(c);                                                             break; // DEC C
    case 0x0E: c = read();                                                         break; // LD C,n
    case 0x11: de = read16();                                                      break; // LD DE,nn
    case 0x13: ++de;                                                               break; // INC DE
    case 0x15: dec(d);                                                             break; // DEC D
    case 0x16: d = read();                                                         break; // LD D,n
    case 0x17: rotateLeft(a);                                                      break; // RLA
    case 0x18: relativeJump(true);                                                 break; // JR n
    case 0x1A: a = mmu->get(de);                                                   break; // LD A,(DE)
    case 0x1D: dec(e);                                                             break; // DEC E
    case 0x1E: e = read();                                                         break; // LD E,n
    case 0x20: relativeJump(f[flagZ]);                                             break; // JR NZ,n
    case 0x21: hl = read16();                                                      break; // LD HL,nn
    case 0x22: mmu->set(hl++, a);                                                  break; // LD (HL+),A
    case 0x23: ++hl;                                                               break; // INC HL
    case 0x24: inc(h);                                                             break; // INC H
    case 0x28: relativeJump(!f[flagZ]);                                            break; // JR Z,n
    case 0x31: sp = read16();                                                      break; // LD SP,nn
    case 0x32: mmu->set(hl--, a);                                                  break; // LDD (HL),A
    case 0x3D: dec(a);                                                             break; // DEC A
    case 0x3E: a = read();                                                         break; // LD A,n
    case 0x4F: c = a;                                                              break; // LD C,A
    case 0x57: d = a;                                                              break; // LD D,A
    case 0x67: h = a;                                                              break; // LD H,A
    case 0x77: mmu->set(hl, a);                                                    break; // LD (HL),A
    case 0x7B: a = e;                                                              break; // LD A,E
    case 0x7C: a = h;                                                              break; // LD A,H
    case 0x90: sub(b);                                                             break; // SUB B
    case 0xAF: xorA(a);                                                            break; // XOR A
    case 0xBE: cp(mmu->get(hl));                                                   break; // CP (HL)
    case 0xC1: bc = pop();                                                         break; // POP BC
    case 0xC9: pc = pop();                                                         break; // RET
    case 0xC5: push(bc);                                                           break; // PUSH BC
    case 0xCB: success = runExtendedCommand();                                     break;
    case 0xCD: call();                                                             break; // CALL nn
    case 0xE0: mmu->set(0xFF00 + read(), a);                                       break; // LDH ($FF00+n),A
    case 0xE2: mmu->set(0xFF00 + c, a);                                            break; // LD ($FF00+C),A
    case 0xEA: mmu->set(read16(), a);                                              break; // LD ($nn),A
    case 0xF0: a = mmu->get(0xFF00 + read());                                      break; // LDH A,(n)
    case 0xFE: cp(read());                                                         break; // CP n
    // clang-format on
    default:
        spdlog::error("Unimplemented opcode: {:02X}", opcode);
        return false;
    }

    return success;
}
