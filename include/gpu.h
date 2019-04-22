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

    std::vector<uint8_t> getScreenBuffer() const;
    std::vector<uint8_t> getTileData() const;

    // Execute GPU step, return true if frame can be drawn.
    bool step();

private:
    Mode mode = Mode::HBlank;

    Mmu& mmu;
    Timer& timer;

    /** Draw tile.
     * buffer - buffer to which tile data will be written
     * tileIndex - index of tile from VRAM to draw
     * tileX - horizontal tile index, where tile will be drawn
     * tileY - vertical tile index, where tile will be drawn
     * w - width of buffer
     * h - height of buffer
     * scx - horizontal tile offset (scroll x)
     * scy - vertical tile offset (scroll y)
     */
    void drawTile(std::vector<uint8_t>& buffer, uint tileIndex, uint tileX, uint tileY, uint w, uint h, uint8_t scx, uint8_t scy) const;
};

#endif // GPU_H
