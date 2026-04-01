#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <fstream>
#include "../types.h"

std::vector<uint8_t> read_file(const std::string& filename);
void write_file(const std::string& filename, const std::vector<Byte>& data);

inline bool is_set(const Byte byte, const Bit bit) { return (byte & static_cast<Byte>(bit)) > 0; }
inline bool is_set(const Word word, const Bit bit) { return is_set(static_cast<Byte>(word), bit); }
inline void set_bit(Byte& byte, const Bit bit) { byte |= static_cast<Byte>(bit); }
inline void reset_bit(Byte& byte, const Bit bit) { byte &= ~static_cast<Byte>(bit); }


#endif  // UTILS_H