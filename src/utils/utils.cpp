#include "utils.h"

/**
 * Read the data stored in a binary file as uint8_ts
 * 
 * @param filename the file containing the data
 * @return a vector of bytes containing the data that was read
*/
std::vector<Byte> read_file(const std::string& filename) {
    std::ifstream infile(filename, std::ios::binary);

    if (!infile.is_open()) {
        throw std::runtime_error("Cannot open file");
    }

    // Get size of file in bytes
    infile.seekg(0, std::ios::end);
    std::streamsize size = infile.tellg();
    infile.seekg(0, std::ios::beg);

    std::vector<Byte> data(size);

    if (!infile.read(reinterpret_cast<char*>(data.data()), (data.size()))) {
        throw std::runtime_error("Failed to read file.");
    }

    infile.close();

    return data;
}


/**
 * Write a vector of uint8_ts to a binary file
 * 
 * @param filename the file to write the data to
 * @param data the data to write to the file
 */
void write_file(const std::string& filename, const std::vector<Byte>& data) {
    std::ofstream outfile(filename, std::fstream::binary);

    if (!outfile.is_open()) {
        throw std::runtime_error("Cannot open file");
    }

    outfile.write(reinterpret_cast<const char*>(data.data()), data.size());
    outfile.close();
}