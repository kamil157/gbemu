#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

// Read whole file into a buffer and return it.
std::vector<uint8_t> readFile(const std::string& path);

// Test if bit n of byte is set.
bool isBitSet(uint8_t byte, uint8_t n);

#endif // UTILS_H
