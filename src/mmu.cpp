#include "mmu.h"

#include <algorithm>
#include <vector>

Mmu::Mmu()
    : memory(0xffff, 0)
{
}

void Mmu::set(uint16_t address, uint8_t value)
{
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
