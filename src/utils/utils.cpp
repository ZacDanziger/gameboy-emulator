#include "utils.h"

/**
 * Read the data stored in a binary file as uint8_t's
 * 
 * @param filename the file containing the data
 * @return a vector of uint8_t's containing the data that was read
*/
std::vector<uint8_t> read_file(const std::string& filename) {
    std::ifstream infile(filename, std::ios::binary);

    if (!infile.is_open()) {
        throw std::runtime_error("Cannot open file");
    }

    // Get size of file in bytes
    infile.seekg(0, std::ios::end);
    std::streamsize size = infile.tellg();
    infile.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(size);

    if (!infile.read(reinterpret_cast<char*>(data.data()), (data.size()))) {
        throw std::runtime_error("Failed to read file.");
    }

    infile.close();

    return data;
}

void write_line(const std::string& filename, const std::string& line) {
    std::ofstream outfile(filename, std::fstream::app);

    if(!outfile.is_open()) {
        throw std::runtime_error("Cannot open_file");
    }

    outfile << line;

    outfile.close();
}