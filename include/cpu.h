#ifndef CPU_H
#define CPU_H

#include "mmu.h"
#include "timer.h"
#include "utils.h"

#include <cstdint>
#include <memory>
#include <ostream>
#include <vector>

#include <json.hpp>

class Cpu {
public:
    Cpu(Mmu& mmu, Timer& timer);

    // Execute next instruction.
    bool execute();

    // Get string representation of cpu state.
    std::string toString() const;

    // Get register values.
    uint16_t getAF() const;
    uint16_t getBC() const;
    uint16_t getDE() const;
    uint16_t getHL() const;
    uint16_t getSP() const;
    uint16_t getPC() const;

    // Set value of AF register to nn.
    void setAF(uint16_t nn);

    // Get flag value from F register.
    bool getFlag(uint8_t flag) const;

    static const uint8_t flagZ = 1 << 7;
    static const uint8_t flagN = 1 << 6;
    static const uint8_t flagH = 1 << 5;
    static const uint8_t flagC = 1 << 4;

private:
    union {
        struct {
            uint8_t f; // Status flags
            uint8_t a; // Accumulator
        };
        uint16_t af;
    };

    // General purpose registers
    union {
        struct {
            uint8_t c;
            uint8_t b;
        };
        uint16_t bc;
    };

    union {
        struct {
            uint8_t e;
            uint8_t d;
        };
        uint16_t de;
    };

    union {
        struct {
            uint8_t l;
            uint8_t h;
        };
        uint16_t hl;
    };

    uint16_t sp = 0; // Stack Pointer
    uint16_t pc = 0; // Program Counter

    bool ime = 0; // Interrupt Master Enable Flag
    std::optional<bool> imeDelayed = 0; // Interrupt Master Enable Flag - if set, sets ime after 1 instruction

    Timer& timer;
    Mmu& mmu;

    // Set flag in register f to b.
    void setFlag(uint8_t flag, bool b);

    // Run Interrupt Service Routine if requested by interrupt flag.
    void handleInterrupts();

    // Run CB prefixed instruction.
    bool executeExtended();

    // Read next byte.
    uint8_t read();

    // Read next 2 bytes.
    uint16_t read16();

    // Decrement reg.
    void dec(uint8_t& reg);

    // Increment reg.
    void inc(uint8_t& reg);

    // xor a with n, store result in a.
    void xorA(uint8_t reg);

    // or a with n, store result in a.
    void orA(uint8_t reg);

    // and a with n, store result in a.
    void andA(uint8_t reg);

    // Pop nn from stack.
    uint16_t pop();

    // Push nn to stack.
    void push(uint16_t nn);

    // Rotate reg right, put old bit 0 in flag C, old flag C in msb.
    void rotateRight(uint8_t& reg);

    // Rotate reg left, put old bit 7 in flag C, old flag C in bit 0.
    void rotateLeft(uint8_t& reg);

    // Shift reg right, put old bit 0 in flag C, 0 in msb.
    void shiftRight(uint8_t& reg);

    // SWAP - Swap upper and lower nibbles of reg.
    void swap(uint8_t& reg);

    // JR n, JR cc,n - add n to current address and jump to it, if condition is met.
    void relativeJump(bool condition);

    // JP nn, JP cc,nn - jump to nn, if condition is met.
    void jump(bool condition);

    // CALL nn, CALL cc, nn - push address of next instruction onto stack and then jump to address nn, if condition is met.
    void call(bool condition);

    // RET, RET cc - pop two bytes from the stack and jump to that address, if condition is met.
    void ret(bool condition);

    // RST n - push pc to stack and jump to target.
    void rst(uint16_t target);

    // CP n - compare a with n.
    void cp(uint8_t n);

    // SUB n - subtract n from a.
    void sub(uint8_t n);

    // ADD A,n - add n to a.
    void add(uint8_t n);

    // ADC A,n - add n + flag C to a.
    void adc(uint8_t n);

    // CPL - flip all bits in a.
    void cpl();

    // ADD HL,nn - add nn to hl
    void addHL(uint16_t nn);

    // BIT n,r - test bit n in register r
    void bit(uint8_t n, uint8_t byte);
};

#endif // CPU_H
