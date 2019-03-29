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
