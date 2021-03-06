#include "cpu.h"

#include "cycles.h"
#include "utils.h"

#include <iostream>

#include "fmt/format.h"
#include "spdlog/spdlog.h"

using json = nlohmann::json;

Cpu::Cpu(Mmu& mmu, Timer& timer)
    : af(0)
    , bc(0)
    , de(0)
    , hl(0)
    , timer(timer)
    , mmu(mmu)
{
}

std::string Cpu::toString() const
{
    return fmt::format("a={:02x} f={:04b} bc={:04x} de={:04x} hl={:04x} sp={:04x} pc={:04x} ", a, f >> 4, bc, de, hl, sp, pc);
}

std::ostream& operator<<(std::ostream& os, Cpu const& cpu)
{
    return os << cpu.toString();
}

bool isFlag(uint8_t flag)
{
    return flag == Cpu::flagZ || flag == Cpu::flagN || flag == Cpu::flagH || flag == Cpu::flagC;
}

void Cpu::setFlag(uint8_t flag, bool b)
{
    if (!isFlag(flag)) {
        throw std::invalid_argument(fmt::format("{:02x} is not a flag value."));
    }
    b ? f |= flag : f &= ~flag;
}

bool Cpu::getFlag(uint8_t flag) const
{
    if (!isFlag(flag)) {
        throw std::invalid_argument(fmt::format("{:02x} is not a flag value."));
    }
    return f & flag;
}

void Cpu::rotateLeft(uint8_t& reg)
{
    bool oldFlagC = getFlag(flagC);
    setFlag(flagC, reg >> 7);
    reg <<= 1;
    reg += oldFlagC;
    setFlag(flagZ, reg == 0);
    setFlag(flagN, 0);
    setFlag(flagH, 0);
}

void Cpu::shiftRight(uint8_t& reg)
{
    setFlag(flagC, reg & 0x1);
    reg >>= 1;
    setFlag(flagZ, reg == 0);
    setFlag(flagN, 0);
    setFlag(flagH, 0);
}

void Cpu::rotateRight(uint8_t& reg)
{
    bool oldFlagC = getFlag(flagC);
    setFlag(flagC, reg & 0x1);
    reg >>= 1;
    reg += oldFlagC << 7;
    setFlag(flagZ, reg == 0);
    setFlag(flagN, 0);
    setFlag(flagH, 0);
}

void Cpu::bit(uint8_t n, uint8_t byte)
{
    setFlag(flagZ, !isBitSet(n, byte));
    setFlag(flagN, 0);
    setFlag(flagH, 1);
}

void Cpu::swap(uint8_t& reg)
{
    uint8_t lower = reg & 0x0f;
    uint8_t upper = reg & 0xf0;
    reg = lower | upper;
    setFlag(flagZ, reg == 0);
    setFlag(flagN, 0);
    setFlag(flagH, 0);
    setFlag(flagC, 0);
}

bool Cpu::executeExtended()
{
    uint8_t opcode = read();
    switch (opcode) {
    // clang-format off
    case 0x11: rotateLeft(c);                                                      break; // RL C
    case 0x19: rotateRight(c);                                                     break; // RR C
    case 0x1A: rotateRight(d);                                                     break; // RR D
    case 0x3F: shiftRight(a);                                                      break; // SRL A
    case 0x37: swap(a);                                                            break; // SWAP A
    case 0x38: shiftRight(b);                                                      break; // SRL B
    case 0x7C: bit(7, h);                                                          break; // BIT 7,H
    // clang-format on
    default:
        spdlog::error("Unimplemented opcode: CB {:02X}", opcode);
        return false;
    }

    return true;
}

uint8_t Cpu::read()
{
    return mmu.get(pc++);
}

uint16_t Cpu::read16()
{
    uint8_t lo = read();
    uint8_t hi = read();
    return concatBytes(lo, hi);
}

uint16_t Cpu::pop()
{
    uint8_t lo = mmu.get(sp++);
    uint8_t hi = mmu.get(sp++);
    return concatBytes(lo, hi);
}

void Cpu::push(uint16_t nn)
{
    mmu.set(--sp, nn >> 8);
    mmu.set(--sp, nn & 0xFF);
}

void Cpu::relativeJump(bool condition)
{
    uint8_t offset = read();
    if (condition) {
        pc += static_cast<int8_t>(offset);
    }
}

void Cpu::jump(bool condition)
{
    uint16_t target = read16();
    if (condition) {
        pc = target;
    }
}

void Cpu::call(bool condition)
{
    uint16_t target = read16();
    if (condition) {
        push(pc);
        pc = target;
    }
}

void Cpu::ret(bool condition)
{
    if (condition) {
        pc = pop();
    }
}

void Cpu::dec(uint8_t& reg)
{
    setFlag(flagH, isHalfCarrySubtraction(static_cast<int8_t>(reg), 1));
    --reg;
    setFlag(flagZ, reg == 0);
    setFlag(flagN, 1);
}

void Cpu::inc(uint8_t& reg)
{
    setFlag(flagH, isHalfCarryAddition(reg, 1));
    ++reg;
    setFlag(flagZ, reg == 0);
    setFlag(flagN, 0);
}

void Cpu::xorA(uint8_t n)
{
    a ^= n;
    setFlag(flagZ, a == 0);
    setFlag(flagN, 0);
    setFlag(flagH, 0);
    setFlag(flagC, 0);
}

void Cpu::orA(uint8_t n)
{
    a |= n;
    setFlag(flagZ, a == 0);
    setFlag(flagN, 0);
    setFlag(flagH, 0);
    setFlag(flagC, 0);
}

void Cpu::andA(uint8_t n)
{
    a &= n;
    setFlag(flagZ, a == 0);
    setFlag(flagN, 0);
    setFlag(flagH, 1);
    setFlag(flagC, 0);
}

void Cpu::cp(uint8_t n)
{
    setFlag(flagZ, a == n);
    setFlag(flagH, isHalfCarrySubtraction(static_cast<int8_t>(a), static_cast<int8_t>(n)));
    setFlag(flagC, a < n);
    setFlag(flagN, 1);
}

void Cpu::sub(uint8_t n)
{
    cp(n);
    a -= n;
}

void Cpu::add(uint8_t n)
{
    setFlag(flagH, isHalfCarryAddition(a, n));
    setFlag(flagC, a + n > 0xff);
    a += n;
    setFlag(flagZ, a == 0);
    setFlag(flagN, 0);
}

void Cpu::addHL(uint16_t nn)
{
    setFlag(flagH, isHalfCarryAddition16(hl, nn));
    setFlag(flagC, hl + nn > 0xffff);
    hl += nn;
    setFlag(flagN, 0);
}

void Cpu::adc(uint8_t n)
{
    add(n + getFlag(flagC));
}

void Cpu::cpl()
{
    a = ~a;
    setFlag(flagN, 1);
    setFlag(flagH, 1);
}

void Cpu::rst(uint16_t target)
{
    push(pc);
    pc = target;
}

uint16_t Cpu::getAF() const { return af; }
uint16_t Cpu::getBC() const { return bc; }
uint16_t Cpu::getDE() const { return de; }
uint16_t Cpu::getHL() const { return hl; }
uint16_t Cpu::getSP() const { return sp; }
uint16_t Cpu::getPC() const { return pc; }

void Cpu::setAF(uint16_t nn) { af = nn; }

void Cpu::handleInterrupts()
{
    auto interruptEnable = mmu.get(0xFFFF);
    auto interruptFlag = mmu.get(0xFF0F);
    for (uint8_t i = 0; i < 5; ++i) {
        if (isBitSet(i, interruptFlag) && ime == 1 && isBitSet(i, interruptEnable)) {
            ime = 0;
            push(pc);
            pc = 0x40 + i * 0x8; // Jump to interrupt handler
            mmu.set(0xFF0F, interruptFlag & ~(1 << i)); // Reset interrupt flag
            timer.increment(20);
            spdlog::debug("Interrupt triggered. Jumping to ${:02x}", pc);
            return;
        }
    }
}

bool Cpu::execute()
{
    bool success = true;

    handleInterrupts();
    if (imeDelayed.has_value()) {
        ime = imeDelayed.value();
        imeDelayed = {};
    }

    timer.increment(getCycles(mmu.get(pc), mmu.get(pc + 1), getFlag(flagZ), getFlag(flagC)));

    uint8_t opcode = read();
    switch (opcode) {
    // clang-format off
    case 0x00:                                                                     break; // NOP
    case 0x01: bc = read16();                                                      break; // LD BC,nn
    case 0x02: mmu.set(bc, a);                                                     break; // LD (BC),A
    case 0x03: ++bc;                                                               break; // INC BC
    case 0x04: inc(b);                                                             break; // INC B
    case 0x05: dec(b);                                                             break; // DEC B
    case 0x06: b = read();                                                         break; // LD B,n
    case 0x0A: a = mmu.get(bc);                                                    break; // LD A,(BC)
    case 0x0B: --bc;                                                               break; // DEC BC
    case 0x0C: inc(c);                                                             break; // INC C
    case 0x0D: dec(c);                                                             break; // DEC C
    case 0x0E: c = read();                                                         break; // LD C,n
    case 0x11: de = read16();                                                      break; // LD DE,nn
    case 0x12: mmu.set(de, a);                                                     break; // LD (DE),A
    case 0x13: ++de;                                                               break; // INC DE
    case 0x14: inc(d);                                                             break; // INC D
    case 0x15: dec(d);                                                             break; // DEC D
    case 0x16: d = read();                                                         break; // LD D,n
    case 0x17: rotateLeft(a);                                                      break; // RLA
    case 0x18: relativeJump(true);                                                 break; // JR n
    case 0x19: addHL(de);                                                          break; // ADD HL,DE
    case 0x1A: a = mmu.get(de);                                                    break; // LD A,(DE)
    case 0x1C: inc(e);                                                             break; // INC E
    case 0x1D: dec(e);                                                             break; // DEC E
    case 0x1E: e = read();                                                         break; // LD E,n
    case 0x1F: rotateRight(a);                                                     break; // RRA
    case 0x20: relativeJump(getFlag(flagZ) == 0);                                  break; // JR NZ,n
    case 0x21: hl = read16();                                                      break; // LD HL,nn
    case 0x22: mmu.set(hl++, a);                                                   break; // LD (HL+),A
    case 0x23: ++hl;                                                               break; // INC HL
    case 0x24: inc(h);                                                             break; // INC H
    case 0x25: dec(h);                                                             break; // DEC H
    case 0x26: h = read();                                                         break; // LD H,n
    case 0x28: relativeJump(getFlag(flagZ) == 1);                                  break; // JR Z,n
    case 0x29: addHL(hl);                                                          break; // ADD HL,HL
    case 0x2A: a = mmu.get(hl++);                                                  break; // LD A,(HL+)
    case 0x2C: inc(l);                                                             break; // INC L
    case 0x2D: dec(l);                                                             break; // DEC L
    case 0x2E: l = read();                                                         break; // LD L,n
    case 0x2F: cpl();                                                              break; // CPL
    case 0x30: relativeJump(getFlag(flagC) == 0);                                  break; // JR NC,n
    case 0x31: sp = read16();                                                      break; // LD SP,nn
    case 0x32: mmu.set(hl--, a);                                                   break; // LDD (HL),A
    case 0x33: ++sp;                                                               break; // INC SP
    case 0x34: ++hl;                                                               break; // INC HL
    case 0x35: { auto n = mmu.get(hl); dec(n); mmu.set(hl, n); }                   break; // DEC (HL)
    case 0x36: mmu.set(hl, read());                                                break; // LD (HL),n
    case 0x37: setFlag(flagN, 0); setFlag(flagH, 0); setFlag(flagC, 1);            break; // SCF
    case 0x38: relativeJump(getFlag(flagC) == 1);                                  break; // JR C,n
    case 0x39: addHL(sp);                                                          break; // ADD HL,SP
    case 0x3A: a = mmu.get(hl--);                                                  break; // LD A,(HL-)
    case 0x3C: inc(a);                                                             break; // INC A
    case 0x3D: dec(a);                                                             break; // DEC A
    case 0x3E: a = read();                                                         break; // LD A,n
    case 0x40: b = b;                                                              break; // LD B,B
    case 0x41: b = c;                                                              break; // LD B,C
    case 0x42: b = d;                                                              break; // LD B,D
    case 0x43: b = e;                                                              break; // LD B,E
    case 0x44: b = h;                                                              break; // LD B,H
    case 0x45: b = l;                                                              break; // LD B,L
    case 0x46: b = mmu.get(hl);                                                    break; // LD B,(HL)
    case 0x47: b = a;                                                              break; // LD B,A
    case 0x48: c = b;                                                              break; // LD C,B
    case 0x49: c = c;                                                              break; // LD C,C
    case 0x4A: c = d;                                                              break; // LD C,D
    case 0x4B: c = e;                                                              break; // LD C,E
    case 0x4C: c = h;                                                              break; // LD C,H
    case 0x4D: c = l;                                                              break; // LD C,L
    case 0x4E: c = mmu.get(hl);                                                    break; // LD C,B
    case 0x4F: c = a;                                                              break; // LD C,A
    case 0x50: d = b;                                                              break; // LD D,B
    case 0x51: d = c;                                                              break; // LD D,C
    case 0x52: d = d;                                                              break; // LD D,D
    case 0x53: d = e;                                                              break; // LD D,E
    case 0x54: d = h;                                                              break; // LD D,H
    case 0x55: d = l;                                                              break; // LD D,L
    case 0x56: d = mmu.get(hl);                                                    break; // LD D,(HL)
    case 0x57: d = a;                                                              break; // LD D,A
    case 0x58: e = b;                                                              break; // LD E,B
    case 0x59: e = c;                                                              break; // LD E,C
    case 0x5A: e = d;                                                              break; // LD E,D
    case 0x5B: e = e;                                                              break; // LD E,E
    case 0x5C: e = h;                                                              break; // LD E,H
    case 0x5D: e = l;                                                              break; // LD E,L
    case 0x5E: e = mmu.get(hl);                                                    break; // LD E,(HL)
    case 0x5F: e = a;                                                              break; // LD E,A
    case 0x60: h = b;                                                              break; // LD H,B
    case 0x61: h = c;                                                              break; // LD H,C
    case 0x62: h = d;                                                              break; // LD H,D
    case 0x63: h = e;                                                              break; // LD H,E
    case 0x64: h = h;                                                              break; // LD H,H
    case 0x65: h = l;                                                              break; // LD H,L
    case 0x66: h = mmu.get(hl);                                                    break; // LD H,(HL)
    case 0x67: h = a;                                                              break; // LD H,A
    case 0x68: l = b;                                                              break; // LD L,B
    case 0x69: l = c;                                                              break; // LD L,C
    case 0x6A: l = d;                                                              break; // LD L,D
    case 0x6B: l = e;                                                              break; // LD L,E
    case 0x6C: l = h;                                                              break; // LD L,H
    case 0x6D: l = l;                                                              break; // LD L,L
    case 0x6E: l = mmu.get(hl);                                                    break; // LD L,(HL)
    case 0x6F: l = a;                                                              break; // LD L,A
    case 0x70: mmu.set(hl, b);                                                     break; // LD (HL),B
    case 0x71: mmu.set(hl, c);                                                     break; // LD (HL),C
    case 0x72: mmu.set(hl, d);                                                     break; // LD (HL),D
    case 0x73: mmu.set(hl, e);                                                     break; // LD (HL),E
    case 0x74: mmu.set(hl, h);                                                     break; // LD (HL),H
    case 0x75: mmu.set(hl, l);                                                     break; // LD (HL),L
    case 0x77: mmu.set(hl, a);                                                     break; // LD (HL),A
    case 0x78: a = b;                                                              break; // LD A,B
    case 0x79: a = c;                                                              break; // LD A,C
    case 0x7A: a = d;                                                              break; // LD A,D
    case 0x7B: a = e;                                                              break; // LD A,E
    case 0x7C: a = h;                                                              break; // LD A,H
    case 0x7D: a = l;                                                              break; // LD A,L
    case 0x7E: a = mmu.get(hl);                                                    break; // LD A,(HL)
    case 0x7F: a = a;                                                              break; // LD A,A
    case 0x86: add(mmu.get(hl));                                                   break; // ADD A,(HL)
    case 0x87: add(a);                                                             break; // ADD A,A
    case 0x90: sub(b);                                                             break; // SUB B
    case 0xA1: andA(c);                                                            break; // AND C
    case 0xA7: andA(a);                                                            break; // AND A
    case 0xA8: xorA(b);                                                            break; // XOR B
    case 0xA9: xorA(c);                                                            break; // XOR C
    case 0xAA: xorA(d);                                                            break; // XOR D
    case 0xAB: xorA(e);                                                            break; // XOR E
    case 0xAC: xorA(h);                                                            break; // XOR H
    case 0xAD: xorA(l);                                                            break; // XOR L
    case 0xAE: xorA(mmu.get(hl));                                                  break; // XOR (HL)
    case 0xAF: xorA(a);                                                            break; // XOR A
    case 0xB0: orA(b);                                                             break; // OR B
    case 0xB1: orA(c);                                                             break; // OR C
    case 0xB2: orA(d);                                                             break; // OR D
    case 0xB3: orA(e);                                                             break; // OR E
    case 0xB4: orA(h);                                                             break; // OR H
    case 0xB5: orA(l);                                                             break; // OR L
    case 0xB6: orA(mmu.get(hl));                                                   break; // OR (HL)
    case 0xB7: orA(a);                                                             break; // OR A
    case 0xBE: cp(mmu.get(hl));                                                    break; // CP (HL)
    case 0xC0: ret(getFlag(flagZ) == 0);                                           break; // RET NZ
    case 0xC1: bc = pop();                                                         break; // POP BC
    case 0xC3: jump(true);                                                         break; // JP nn
    case 0xC4: call(getFlag(flagZ) == 0);                                          break; // CALL NZ,nn
    case 0xC6: add(read());                                                        break; // ADD A,n
    case 0xC8: ret(getFlag(flagZ) == 1);                                           break; // RET Z
    case 0xC9: ret(true);                                                          break; // RET
    case 0xC5: push(bc);                                                           break; // PUSH BC
    case 0xCA: jump(getFlag(flagZ) == 1);                                          break; // JP Z
    case 0xCB: success = executeExtended();                                        break;
    case 0xCC: call(getFlag(flagZ) == 1);                                          break; // CALL Z,nn
    case 0xCD: call(true);                                                         break; // CALL nn
    case 0xCE: adc(read());                                                        break; // ADC A,n
    case 0xD0: ret(getFlag(flagC) == 0);                                           break; // RET NC
    case 0xD1: de = pop();                                                         break; // POP DE
    case 0xD4: call(getFlag(flagC) == 0);                                          break; // CALL NC,nn
    case 0xD5: push(de);                                                           break; // PUSH DE
    case 0xD6: sub(read());                                                        break; // SUB N
    case 0xD8: ret(getFlag(flagC) == 1);                                           break; // RET C
    case 0xD9: imeDelayed = 1; ret(true);                                          break; // RETI
    case 0xDC: call(getFlag(flagC) == 1);                                          break; // CALL C,nn
    case 0xE0: mmu.set(0xFF00 + read(), a);                                        break; // LDH ($FF00+n),A
    case 0xE1: hl = pop();                                                         break; // POP HL
    case 0xE2: mmu.set(0xFF00 + c, a);                                             break; // LD ($FF00+C),A
    case 0xE5: push(hl);                                                           break; // PUSH HL
    case 0xE6: andA(read());                                                       break; // AND n
    case 0xE9: pc = mmu.get(hl);                                                   break; // JP (HL)
    case 0xEA: mmu.set(read16(), a);                                               break; // LD ($nn),A
    case 0xEE: xorA(read());                                                       break; // XOR n
    case 0xEF: rst(0x0028);                                                        break; // RST 28H
    case 0xF0: a = mmu.get(0xFF00 + read());                                       break; // LDH A,(n)
    case 0xF1: setAF(pop());                                                       break; // POP AF
    case 0xF3: imeDelayed = 0;                                                     break; // DI
    case 0xF5: push(getAF());                                                      break; // PUSH AF
    case 0xF6: orA(read());                                                        break; // OR n
    case 0xFA: a = mmu.get(read16());                                              break; // LD A,(nn)
    case 0xFB: imeDelayed = 1;                                                     break; // EI
    case 0xFE: cp(read());                                                         break; // CP n
    case 0xFF: rst(0x0038);                                                        break; // RST 38H
    // clang-format on
    default:
        spdlog::error("Unimplemented opcode: {:02X}", opcode);
        return false;
    }

    return success;
}
