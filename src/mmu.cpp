#include "mmu.h"

#include <algorithm>
#include <vector>

#include <spdlog/spdlog.h>

Mmu::Mmu()
    : memory(0xffff, 0)
{
}

void Mmu::set(uint16_t address, uint8_t value)
{
    // Writing 0x81 to 0xFF02 transfers value from 0xFF01 over serial link.
    // Emulate this by logging it.
    if (address == 0xFF02 && value == 0x81) {
        spdlog::info("{:02x}", get(0xFF01));
    }
    memory.at(address) = value;
}

uint8_t Mmu::get(uint16_t address) const
{
    return memory.at(address);
}

void Mmu::load(uint16_t address, const byteCodePtr& code)
{
    memory.insert(memory.begin() + address, code->begin(), code->end());
}
