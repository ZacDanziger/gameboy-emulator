#include "logger.h"

Logger::Logger() {
    line = "";
    filename = "";
}

void Logger::update_filename(const std::string& file) {
    filename = file;

    //clear the file before starting to log
    std::ofstream outfile(filename, std::ios::trunc);
    outfile.close();
}


CPULogger::CPULogger(CPU* cpu_) {
    cpu = cpu_;

    std::ofstream outfile(buffer_filename, std::ios::trunc);
    outfile.close();
}


void CPULogger::log() {
    update_line();
    write_line(filename, line);

    log_buffer();
}


void CPULogger::log_buffer() {
    // only want last instance of serial buffer
    std::ofstream outfile(buffer_filename, std::ios::trunc);
    outfile.close();
    write_line(buffer_filename, cpu->serial_buffer);
}



void CPULogger::update_line() {
    std::stringstream output;
    output << std::hex << std::setfill('0');

    output << "A:" << std::hex << std::setw(2) << static_cast<int>(cpu->reg_A)
           << " F:" << std::hex << std::setw(2) << static_cast<int>(cpu->reg_F)
           << " B:" << std::hex << std::setw(2) << static_cast<int>(cpu->reg_B)
           << " C:" << std::hex << std::setw(2) << static_cast<int>(cpu->reg_C)
           << " D:" << std::hex << std::setw(2) << static_cast<int>(cpu->reg_D)
           << " E:" << std::hex << std::setw(2) << static_cast<int>(cpu->reg_E)
           << " H:" << std::hex << std::setw(2) << static_cast<int>(cpu->reg_H)
           << " L:" << std::hex << std::setw(2) << static_cast<int>(cpu->reg_L)
           << " SP:" << std::hex << std::setw(4) << static_cast<int>(cpu->reg_SP)
           << " PC:" << std::hex << std::setw(4) << static_cast<int>(cpu->reg_PC)
           << " PCMEM:" << std::hex << std::setw(2) << static_cast<int>(cpu->memory->read(cpu->reg_PC))
           << "," << std::hex << std::setw(2) << static_cast<int>(cpu->memory->read(cpu->reg_PC+1))
           << "," << std::hex << std::setw(2) << static_cast<int>(cpu->memory->read(cpu->reg_PC+2))
           << "," << std::hex << std::setw(2) << static_cast<int>(cpu->memory->read(cpu->reg_PC+3))
           << '\n';
    line = output.str();
}