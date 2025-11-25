#include "logger.h"

Logger::Logger() {
    buffer = "";
    line_size = 0;
}

Logger::~Logger() {
    if (!buffer.empty()) {
        out << buffer;
    }
    if (out.is_open()) {
        out.close();
    }

}

void Logger::init_ofstream(const std::string& filename) {
    if (out.is_open()) {
        out.close();
    }

    out.open(filename, std::ios::trunc);

    if (!out.is_open()) {
        throw std::runtime_error("Cannot open file");
    }
    out.close();

    out.open(filename, std::ios::app);
}

void Logger::log() {
    update_line();
    if (out.is_open()) {
        write_line(out, buffer);
        buffer.clear();
    }
}


CPULogger::CPULogger(CPU* cpu_) {
    cpu = cpu_;
}

void CPULogger::log_serial_buffer(const std::string& buffer_filename) {
    std::ofstream buffer(buffer_filename);

    write_line(buffer, cpu->serial_buffer);
    buffer.close();
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
           << " PCMEM:" << std::hex << std::setw(2) << static_cast<int>(cpu->mmu->read(cpu->reg_PC))
           << "," << std::hex << std::setw(2) << static_cast<int>(cpu->mmu->read(cpu->reg_PC+1))
           << "," << std::hex << std::setw(2) << static_cast<int>(cpu->mmu->read(cpu->reg_PC+2))
           << "," << std::hex << std::setw(2) << static_cast<int>(cpu->mmu->read(cpu->reg_PC+3))
           << '\n';

    if (line_size == 0) {
        line_size = output.str().size();
    }
    buffer += output.str();
}

TimerLogger::TimerLogger(Timer* timer_ptr) {
    timer = timer_ptr;
}

void TimerLogger::update_line() {
    std::stringstream output;
    output << std::hex << std::setfill('0');

    output << "Div Internal:" << std::hex << std::setw(4) << static_cast<int>(timer->divider_internal)
           << " DIV:" << std::hex << std::setw(2) << static_cast<int>(timer->divider)
           << " TIMA:" << std::hex << std::setw(2) << static_cast<int>(timer->timer)
           << " TMA:" << std::hex << std::setw(2) << static_cast<int>(timer->timer_modulo)
           << " TAC:" << std::hex << std::setw(2) << static_cast<int>(timer->timer_control)
           << " Div Bit:" << std::hex << std::setw(2) << static_cast<int>(timer->div_bit)
           << " Enabled:" << std::hex << std::setw(2) << static_cast<int>(timer->enabled)
           << " Prev High:" << std::hex << std::setw(2) << static_cast<int>(timer->previous_high)
           << " Overflow Delay:" << std::hex << std::setw(2) << static_cast<int>(timer->overflow_delay)
           << " Reload Cycle:" << std::hex << std::setw(2) << static_cast<int>(timer->timer_reload_cycle)
           << " IF REG:" << std::hex << std::setw(2) << static_cast<int>(timer->read(IF_REGISTER))
           << " IE REG:" << std::hex << std::setw(2) << static_cast<int>(timer->read(IE_REGISTER))
           << '\n';
    if (line_size == 0) {
        line_size = output.str().size();
    }
    buffer += output.str();
}