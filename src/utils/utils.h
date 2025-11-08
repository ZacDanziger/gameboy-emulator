#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <cstdint>
#include <string>
#include <fstream>

std::vector<uint8_t> read_file(const std::string& filename);
void write_line(const std::string& filename, const std::string& line);

#endif  // UTILS_H