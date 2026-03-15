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


/**
 * Append a line to an output file
 * 
 * @param filename the file to append to
 * @param line the line to append
*/
void write_line(std::ofstream& out, const std::string& line) {
    if(!out.is_open()) {
        throw std::runtime_error("Cannot open_file");
    }

    out << line;
}

void dump(const std::string& filename) {
    const int BYTES_PER_LINE = 16;

    std::vector<uint8_t> data = read_file(filename);

    std::ofstream outfile("/Users/zacdanziger/Documents/Personal/Coding/gameboy-emulator/build/rom_dump.txt");
    if (!outfile.is_open()) {
        throw std::runtime_error("Cannot open out file");
    }

    outfile << "Size: " << data.size() << " bytes (0x" 
            << std::hex << std::uppercase << data.size() << std::dec << ")\n";
    outfile << std::string(70, '-') << "\n";
    outfile << "Address  | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F | ASCII\n";
    outfile << std::string(70, '-') << "\n";

    for (size_t i = 0; i < data.size(); i += BYTES_PER_LINE) {
        // Address
        outfile << "0x" << std::hex << std::uppercase 
                << std::setw(4) << std::setfill('0') << i << "   | ";
 
        // Hex bytes
        for (int j = 0; j < BYTES_PER_LINE; j++) {
            if (i + j < data.size()) {
                outfile << std::hex << std::uppercase
                        << std::setw(2) << std::setfill('0') 
                        << static_cast<int>(data[i + j]) << " ";
            } else {
                outfile << "   ";  // pad if last line is short
            }
        }
 
        outfile << "| ";
 
        // // ASCII representation
        // for (int j = 0; j < BYTES_PER_LINE && i + j < data.size(); j++) {
        //     char c = static_cast<char>(data[i + j]);
        //     outfile << (std::isprint(static_cast<unsigned char>(c)) ? c : '.');
        // }
 
        outfile << "\n";
    }

    outfile << std::string(70, '-') << "\n";

    outfile.close();
}