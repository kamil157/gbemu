#ifndef GPU_H
#define GPU_H

#include "mmu.h"
#include "timer.h"

#include <stdint.h>

enum class Mode {
    HBlank,
    VBlank,
    OamAccess,
    VramAccress
};

class Gpu {
public:
    Gpu(Mmu& mmu, Timer& timer);

    // Execute GPU step, return true if frame can be drawn.
    bool step();

private:
    Mode mode = Mode::HBlank;

    Mmu& mmu;
    Timer& timer;
};

#endif // GPU_H
