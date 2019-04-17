#include "gpu.h"

#include <spdlog/spdlog.h>

Gpu::Gpu(Mmu& mmu, Timer& timer)
    : mmu(mmu)
    , timer(timer)
{
}

void Gpu::step()
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
                spdlog::info("VBlank done.");
                // Restart scanning modes
                mode = Mode::OamAccess;
                mmu.set(lineAddress, 0);
            }
        }
        break;
    }
}
