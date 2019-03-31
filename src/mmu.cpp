#include "mmu.h"

Mmu::Mmu()
    : ram(0xffff, 0)
{
}

void Mmu::set(uint16_t address, uint8_t value)
{
    ram.at(address) = value;
}

uint8_t Mmu::get(uint16_t address) const
{
    return ram.at(address);
}
