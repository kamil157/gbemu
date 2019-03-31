#ifndef MMU_H
#define MMU_H

#include <cstdint>
#include <vector>

class Mmu {
public:
    Mmu();
    void set(uint16_t address, uint8_t value);
    uint8_t get(uint16_t address) const;

private:
    std::vector<uint8_t> ram;
};

#endif // MMU_H
