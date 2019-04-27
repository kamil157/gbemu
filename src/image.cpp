#include "image.h"

Image::Image(unsigned int width, unsigned int height)
    : width(width)
    , height(height)
    , data(bytesPerPixel * width * height, 0)
{
}

static const std::vector<Color> palette = { { 255, 255, 255 }, { 170, 170, 170 }, { 85, 85, 85 }, { 0, 0, 0 } };

const uint8_t* Image::getData() const
{
    return data.data();
}

void Image::drawPixel(unsigned int x, unsigned int y, Color color)
{
    data[4 * (y * width + x)] = color.r;
    data[4 * (y * width + x) + 1] = color.g;
    data[4 * (y * width + x) + 2] = color.b;
}

void Image::drawLine(uint8_t byte0, uint8_t byte1, unsigned int tileX, unsigned int tileY, uint8_t scx, uint8_t scy, uint8_t line)
{
    for (auto pixel = pixelsPerLine - 1; pixel >= 0; --pixel) {
        auto bit1 = (byte1 >> pixel) & 1;
        auto bit0 = (byte0 >> pixel) & 1;
        auto colorIndex = (bit0 << 1) + bit1;

        auto color = palette.at(static_cast<uint8_t>(colorIndex));
        auto x = static_cast<int>(tileX * pixelsPerLine + pixelsPerLine - 1 - static_cast<unsigned int>(pixel) - scx);
        auto y = static_cast<int>(tileY * linesPerTile + line - scy);
        if (y < 0 || y >= static_cast<int>(height) || x < 0 || x >= static_cast<int>(width)) {
            continue;
        }
        drawPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), color);
    }
}
