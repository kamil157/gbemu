#include "utils.h"

#include <fstream>

byteCodePtr readFile(const std::string& path)
{
    std::ifstream input(path, std::ios::binary);
    if (input.fail()) {
        throw std::runtime_error("Couldn't open file: " + path);
    }

    // Copies all data into buffer
    return std::make_shared<std::vector<uint8_t>>(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

bool isBitSet(uint8_t byte, uint8_t n)
{
    return (byte >> n) & 1;
}
