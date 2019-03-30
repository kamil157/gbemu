#ifndef MMU_H
#define MMU_H

#include <vector>
#include <cstdint>

class Mmu
{
public:
Mmu();
void set(uint16_t address, uint8_t value);
uint8_t get(uint16_t address);

private:
    std::vector<uint8_t> ram;
};

#endif // MMU_H
