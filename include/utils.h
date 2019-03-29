#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>

// Read whole file into a buffer and return it.
std::vector<uint8_t> readFile(const std::string& path);

#endif // UTILS_H
