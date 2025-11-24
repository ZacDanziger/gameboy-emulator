#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <cstdint>
#include <string>
#include <fstream>

std::vector<uint8_t> read_file(const std::string& filename);
void write_line(std::ofstream& out, const std::string& line);

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

#endif  // UTILS_H