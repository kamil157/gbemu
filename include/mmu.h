#ifndef MMU_H
#define MMU_H

#include "utils.h"

#include <cstdint>
#include <gsl/span>
#include <vector>

class Mmu {
public:
    Mmu();

    // Set memory value at address.
    void set(uint16_t address, uint8_t value);

    // Get memory value at address.
    uint8_t get(uint16_t address) const;

    // Load bootstrap ROM into memory.
    void loadBootstrap(const std::vector<uint8_t>& rom);

    // Load cartridge ROM into memory.
    void loadCartridge(const std::vector<uint8_t>& cartridgeStart);

    // Return a view of VRAM data.
    const gsl::span<const uint8_t> getVram() const;

    // Return contents of memory.
    std::vector<uint8_t> getMemory() const;

private:
    std::vector<uint8_t> memory;
    std::vector<uint8_t> cartridgeStart;
};

#endif // MMU_H
