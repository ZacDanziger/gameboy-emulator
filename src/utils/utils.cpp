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


// // For debugging purposes -- gameboy-doctor
// void CPU::write_registers() {
//     std::stringstream output;

//     output << std::hex << std::setfill('0');

//     output << "A:" << std::hex << std::setw(2) << static_cast<int>(reg_A)
//            << " F:" << std::hex << std::setw(2) << static_cast<int>(reg_F)
//            << " B:" << std::hex << std::setw(2) << static_cast<int>(reg_B)
//            << " C:" << std::hex << std::setw(2) << static_cast<int>(reg_C)
//            << " D:" << std::hex << std::setw(2) << static_cast<int>(reg_D)
//            << " E:" << std::hex << std::setw(2) << static_cast<int>(reg_E)
//            << " H:" << std::hex << std::setw(2) << static_cast<int>(reg_H)
//            << " L:" << std::hex << std::setw(2) << static_cast<int>(reg_L)
//            << " SP:" << std::hex << std::setw(4) << static_cast<int>(reg_SP)
//            << " PC:" << std::hex << std::setw(4) << static_cast<int>(reg_PC)
//            << " PCMEM:" << std::hex << std::setw(2) << static_cast<int>(_memory->read(reg_PC))
//            << "," << std::hex << std::setw(2) << static_cast<int>(_memory->read(reg_PC+1))
//            << "," << std::hex << std::setw(2) << static_cast<int>(_memory->read(reg_PC+2))
//            << "," << std::hex << std::setw(2) << static_cast<int>(_memory->read(reg_PC+3))
//            << '\n';

//     write_line(_logfile, output.str());
// }
