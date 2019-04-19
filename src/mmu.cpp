#include "mmu.h"

#include <algorithm>
#include <vector>

#include <spdlog/spdlog.h>

Mmu::Mmu()
    : memory(0x10000, 0)
{
    //    memory[0xff44] = 0x90; // Don't wait for VBLANK as it's not implemented.
}

void Mmu::set(uint16_t address, uint8_t value)
{
    // Writing 0x81 to 0xFF02 transfers value from 0xFF01 over serial link.
    // Emulate this by logging it.
    if (address == 0xFF02 && value == 0x81) {
        spdlog::info("{:02x}", get(0xFF01));
    }
    // Writing the value of 1 to the address 0xFF50 unmaps the boot ROM.
    if (address == 0xFF50 && value == 0x01) {
        spdlog::info("Unmapping boot ROM.");
        loadBootstrap(cartridgeStart);
    }
    memory.at(address) = value;
}

uint8_t Mmu::get(uint16_t address) const
{
    return memory.at(address);
}

void Mmu::loadBootstrap(const std::vector<uint8_t>& rom)
{
    memory.erase(memory.begin(), memory.begin() + 0x100);
    memory.insert(memory.begin(), rom.begin(), rom.end());
}

void Mmu::loadCartridge(const std::vector<uint8_t>& rom)
{
    auto const allowedSize = 0x8000;
    if (rom.size() > allowedSize) {
        spdlog::warn("Cartridge size not supported. Supported size: 0x{:x}, given size: 0x{:x}", allowedSize, rom.size());
    }
    // Don't overwrite the boot ROM.
    cartridgeStart = std::vector<uint8_t>{ rom.begin(), rom.begin() + 0x100 };
    auto size = static_cast<int>(rom.size());
    memory.erase(memory.begin() + 0x100, memory.begin() + size);
    memory.insert(memory.begin() + 0x100, rom.begin() + 0x100, rom.end());
}

const gsl::span<const uint8_t> Mmu::getVram() const
{
    // VRAM is between 0x8000 and 0xA000
    return gsl::make_span(memory).subspan(0x8000, 0x2000);
}

std::vector<uint8_t> Mmu::getMemory() const
{
    return memory;
}
