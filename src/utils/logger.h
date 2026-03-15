#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include "utils.h"
#include "../cpu/cpu.h"

class Logger {
    public:
        Logger();
        ~Logger();

        void log();
        void init_ofstream(const std::string& filename);
        void teardown() { out.close(); }
    protected:
        virtual void update_line() {}
        std::string buffer;
        std::ofstream out;
        size_t line_size;
};


class CPULogger: public Logger {
    public:
        CPULogger(CPU* cpu_ptr);
        void log_serial_buffer(const std::string& buffer_filename);
    private:
        void update_line() override;
        CPU *cpu;
};

class TimerLogger: public Logger {
    public:
        TimerLogger(Timer* timer_ptr);
    private:
        void update_line() override;
        Timer *timer;
};

#endif