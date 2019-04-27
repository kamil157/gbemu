#ifndef GPU_H
#define GPU_H

#include "image.h"
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

    Image getTileData() const;
    Image getBgMap() const;
    Image getScreenBuffer() const;

    // Execute GPU step, return true if frame can be drawn.
    bool step();

private:
    Mode mode = Mode::HBlank;

    Mmu& mmu;
    Timer& timer;

    /** Draw tile.
     * image - image to which tile data will be written
     * tileIndex - index of tile from VRAM to draw
     * tileX - horizontal tile index, where tile will be drawn
     * tileY - vertical tile index, where tile will be drawn
     * scx - horizontal tile offset (scroll x)
     * scy - vertical tile offset (scroll y)
     */
    void drawTile(Image& image, uint tileIndex, uint tileX, uint tileY, uint8_t scx, uint8_t scy) const;
};

#endif // GPU_H
