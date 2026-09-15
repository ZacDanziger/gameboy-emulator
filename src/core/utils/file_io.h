#ifndef FILE_IO_H
#define FILE_IO_H

#include <vector>
#include <string>
#include <fstream>

#include "../types.h"

std::vector<uint8_t> read_file(const std::string& filename);
void write_file(const std::string& filename, const std::vector<Byte>& data);

#endif  // FILE_IO_H