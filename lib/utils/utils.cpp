#include "utils.h"

#include <fstream>

std::vector<uint8_t> readFile(const std::string& path)
{
    std::ifstream input(path, std::ios::binary);
    if (input.fail()) {
        throw std::runtime_error("Couldn't open file: " + path);
    }

    // copies all data into buffer
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(input), {});
    return buffer;
}

bool isBitSet(uint8_t byte, uint8_t n)
{
    return (byte >> n) & 1;
}
