#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <fstream>
#include "../types.h"

std::vector<uint8_t> read_file(const std::string& filename);

inline bool is_set(const Byte byte, const Bit bit) { return (byte & static_cast<Byte>(bit)) > 0; }
inline bool is_set(const Word word, const Bit bit) { return is_set(static_cast<Byte>(word), bit); }
inline void set_bit(Byte& byte, const Bit bit) { byte |= static_cast<Byte>(bit); }
inline void reset_bit(Byte& byte, const Bit bit) { byte &= ~static_cast<Byte>(bit); }


// functions for testing //

void write_line(std::ofstream& out, const std::string& line);

/**
 * Create a [size] number of random bytes, and write that "numbers.txt"
 */
template <size_t size>
void create_data() {
    std::array<uint8_t, size> array;
    std::srand(std::time(NULL));
    std::ofstream out("numbers.txt", std::fstream::binary);

    if (!out.is_open()) {
        throw std::runtime_error("Cannot open file");
    }


    for (int i = 0; i < size; i++) {
        array[i] = static_cast<uint8_t> (std::rand() % 0xFF);
    }

    out.write(reinterpret_cast<const char*>(array.data()), array.size());
    out.close();
}

void dump(const std::string& filename);

#endif  // UTILS_H