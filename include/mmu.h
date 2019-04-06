#ifndef MMU_H
#define MMU_H

#include "utils.h"

#include <cstdint>
#include <vector>

class Mmu {
public:
    Mmu();

    // Set memory value at address.
    void set(uint16_t address, uint8_t value);

    // Get memory value at address.
    uint8_t get(uint16_t address) const;

    // Load code into memory starting at address.
    void load(uint16_t address, const byteCodePtr& code);

    // Return a copy of VRAM data.
    std::vector<uint8_t> getVram() const;

private:
    std::vector<uint8_t> memory;
};

#endif // MMU_H
