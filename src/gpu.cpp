#include "gpu.h"

#include <spdlog/spdlog.h>

Gpu::Gpu(Mmu& mmu, Timer& timer)
    : mmu(mmu)
    , timer(timer)
{
}

static const uint8_t visiblePixelsX = 160;
static const uint8_t visiblePixelsY = 144;

void Gpu::drawTile(Image& image, uint tileIndex, uint tileX, uint tileY, uint8_t scx, uint8_t scy) const
{
    for (uint8_t line = 0u; line < linesPerTile; ++line) {
        auto index = static_cast<uint16_t>(2 * (line + linesPerTile * tileIndex)); // location of tile data in vram
        auto byte1 = mmu.getVram().at(index);
        auto byte0 = mmu.getVram().at(index + 1);
        image.drawLine(byte0, byte1, tileX, tileY, scx, scy, line);
    }
}

Image Gpu::getTileData() const
{
    static const auto tilesPerRow = 16u;
    static const auto tilesPerColumn = 32u;

    Image image{ tilesPerRow * pixelsPerLine, tilesPerColumn * linesPerTile };
    for (auto tileY = 0u; tileY < tilesPerColumn; ++tileY) {
        for (auto tileX = 0u; tileX < tilesPerRow; ++tileX) {
            auto tileIndex = tileX + tilesPerRow * tileY;
            drawTile(image, tileIndex, tileX, tileY, 0, 0);
        }
    }
    return image;
}

Image Gpu::getBgMap() const
{
    static const auto tilesPerRow = 32u;
    static const auto tilesPerColumn = 32u;

    auto vram = mmu.getVram();
    Image image{ tilesPerRow * pixelsPerLine, tilesPerColumn * linesPerTile };
    for (auto tileY = 0u; tileY < tilesPerColumn; ++tileY) {
        for (auto tileX = 0u; tileX < tilesPerRow; ++tileX) {
            auto tileOffset = tileY * tilesPerRow + tileX; // tile offset in bg map
            auto bgMapOffset = 0x1800u; // or 0x1C00
            auto tileIndex = vram[bgMapOffset + tileOffset]; // index of tile to draw here
            drawTile(image, tileIndex, tileX, tileY, 0, 0);
        }
    }
    // Draw camera position
    auto scx = mmu.get(0xff43);
    auto scy = mmu.get(0xff42);
    Color black{ 0, 0, 0 };
    for (uint8_t x = 0; x <= visiblePixelsX; ++x) {
        image.drawPixel(static_cast<uint>(x + scx), static_cast<uint>(scy), black);
        image.drawPixel(static_cast<uint>(x + scx), static_cast<uint>(scy + visiblePixelsY), black);
    }
    for (uint8_t y = 0; y <= visiblePixelsY; ++y) {
        image.drawPixel(static_cast<uint>(scx), static_cast<uint>(y + scy), black);
        image.drawPixel(static_cast<uint>(scx + visiblePixelsX), static_cast<uint>(y + scy), black);
    }
    return image;
}

Image Gpu::getScreenBuffer() const
{
    static const auto tilesPerRow = 32u;
    static const auto tilesPerColumn = 32u;

    auto vram = mmu.getVram();
    Image image{ visiblePixelsX, visiblePixelsY };
    for (auto tileY = 0u; tileY < tilesPerColumn; ++tileY) {
        for (auto tileX = 0u; tileX < tilesPerRow; ++tileX) {
            auto tileOffset = tileY * tilesPerRow + tileX; // tile offset in bg map
            auto bgMapOffset = 0x1800u; // or 0x1C00
            auto tileIndex = vram[bgMapOffset + tileOffset]; // index of tile to draw here
            drawTile(image, tileIndex, tileX, tileY, mmu.get(0xff43), mmu.get(0xff42));
        }
    }
    return image;
}

bool Gpu::step()
{
    static const int lineAddress = 0xff44;
    int clock = timer.getCycles();
    switch (mode) {

    // OAM read mode, scanline active
    case Mode::OamAccess:
        if (clock >= 80) {
            // Enter scanline mode 3
            timer.reset();
            mode = Mode::VramAccress;
        }
        break;

    // VRAM read mode, scanline active
    // Treat end of mode 3 as end of scanline
    case Mode::VramAccress:
        if (clock >= 172) {
            // Enter hblank
            timer.reset();
            mode = Mode::HBlank;

            // Write a scanline to the framebuffer
            //            renderscan();
        }
        break;

    // Hblank
    // After the last hblank, push the screen data to canvas
    case Mode::HBlank:
        if (clock >= 204) {
            timer.reset();
            mmu.set(lineAddress, mmu.get(lineAddress) + 1);

            if (mmu.get(lineAddress) == 143) {
                // Enter vblank
                mode = Mode::VBlank;
                //                GPU._canvas.putImageData(GPU._scrn, 0, 0);
            } else {
                mode = Mode::OamAccess;
            }
        }
        break;

    // Vblank (10 lines)
    case Mode::VBlank:
        if (clock >= 456) {
            timer.reset();
            mmu.set(lineAddress, mmu.get(lineAddress) + 1);

            if (mmu.get(lineAddress) > 153) {
                spdlog::trace("VBlank done.");
                // Restart scanning modes
                mode = Mode::OamAccess;
                mmu.set(lineAddress, 0);
                return true;
            }
        }
        break;
    }
    return false;
}
