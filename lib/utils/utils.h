#ifndef UTILS_H
#define UTILS_H

#include <memory>
#include <string>
#include <vector>

using byteCodePtr = std::shared_ptr<std::vector<uint8_t>>;

// Read whole file into a buffer and return it.
byteCodePtr readFile(const std::string& path);

// Test if bit n of byte is set.
bool isBitSet(uint8_t byte, uint8_t n);

#endif // UTILS_H
