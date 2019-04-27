#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <vector>

struct Color {
    uint8_t r, g, b;
};

static const auto bytesPerPixel = 4;
static const auto linesPerTile = 8;
static const auto pixelsPerLine = 8;

class Image {
public:
    Image(unsigned int width, unsigned int height);

    const uint8_t* getData() const;

    // Draw a pixel with color at (x, y).
    void drawPixel(unsigned int x, unsigned int y, Color color);

    // Draw a single line of a tile.
    void drawLine(uint8_t byte0, uint8_t byte1, unsigned int tileX, unsigned int tileY, uint8_t scx, uint8_t scy, uint8_t line);

private:
    unsigned int width;
    unsigned int height;
    std::vector<uint8_t> data;
};

#endif // IMAGE_H
