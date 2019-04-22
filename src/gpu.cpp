#include "gpu.h"

#include <spdlog/spdlog.h>

Gpu::Gpu(Mmu& mmu, Timer& timer)
    : mmu(mmu)
    , timer(timer)
{
}

struct Color {
    uint8_t r, g, b;
};

static const std::vector<Color> palette = { { 255, 255, 255 }, { 170, 170, 170 }, { 85, 85, 85 }, { 0, 0, 0 } };

void Gpu::drawTile(std::vector<uint8_t>& buffer, uint tileIndex, uint tileX, uint tileY, uint w, uint h, uint8_t scx, uint8_t scy) const
{
    static const auto linesPerTile = 8;
    static const auto pixelsPerLine = 8;

    for (auto line = 0u; line < linesPerTile; ++line) {
        auto index = static_cast<uint16_t>(2 * (line + linesPerTile * tileIndex)); // location of tile data in vram
        auto byte1 = mmu.getVram().at(index);
        auto byte0 = mmu.getVram().at(index + 1);
        for (auto pixel = pixelsPerLine - 1; pixel >= 0; --pixel) {
            auto bit1 = (byte1 >> pixel) & 1;
            auto bit0 = (byte0 >> pixel) & 1;
            auto colorIndex = (bit0 << 1) + bit1;

            auto color = palette.at(static_cast<uint8_t>(colorIndex));
            auto x = static_cast<int>(tileX * pixelsPerLine + pixelsPerLine - 1 - static_cast<uint>(pixel) - scx);
            auto y = static_cast<int>(tileY * linesPerTile + line - scy);
            if (y >= 0 && y < static_cast<int>(h) && x >= 0 && x < static_cast<int>(w)) {
                buffer[4 * (static_cast<uint>(y) * w + static_cast<uint>(x))] = color.r;
                buffer[4 * (static_cast<uint>(y) * w + static_cast<uint>(x)) + 1] = color.g;
                buffer[4 * (static_cast<uint>(y) * w + static_cast<uint>(x)) + 2] = color.b;
            }
        }
    }
}

std::vector<uint8_t> Gpu::getTileData() const
{
    static const auto tilesPerRow = 16;
    static const auto tilesPerColumn = 32;
    static const auto bytesPerPixel = 4;

    auto w = 128u;
    auto h = 256u;
    std::vector<uint8_t> buffer(bytesPerPixel * w * h, 0);
    for (auto tileY = 0u; tileY < tilesPerColumn; ++tileY) {
        for (auto tileX = 0u; tileX < tilesPerRow; ++tileX) {
            auto tileIndex = tileX + tilesPerRow * tileY;
            drawTile(buffer, tileIndex, tileX, tileY, w, h, 0, 0);
        }
    }
    return buffer;
}

std::vector<uint8_t> Gpu::getScreenBuffer() const
{
    static const auto tilesPerRow = 32;
    static const auto tilesPerColumn = 32;
    static const auto bytesPerPixel = 4;

    auto vram = mmu.getVram();
    auto w = 160u;
    auto h = 144u;
    std::vector<uint8_t> buffer(bytesPerPixel * w * h, 0);
    for (auto tileY = 0u; tileY < tilesPerColumn; ++tileY) {
        for (auto tileX = 0u; tileX < tilesPerRow; ++tileX) {
            auto tileOffset = tileY * tilesPerRow + tileX; // tile offset in bg map
            auto bgMapOffset = 0x1800u; // or 0x1C00
            auto tileIndex = vram[bgMapOffset + tileOffset]; // index of tile to draw here
            drawTile(buffer, tileIndex, tileX, tileY, w, h, mmu.get(0xff43), mmu.get(0xff42));
        }
    }
    return buffer;
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
