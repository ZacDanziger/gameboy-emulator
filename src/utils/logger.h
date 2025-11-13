#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include "../cpu/cpu.h"

class Logger {
    public:
        Logger();

        void update_filename(const std::string& file);
        virtual void log() {}

    protected:
        virtual void update_line() {}
        std::string filename;
        std::string line;
};


class CPULogger: public Logger {
    public:
        CPULogger(CPU* cpu_);
        void log() override;
        void log_buffer();
    private:
        void update_line() override;
        CPU *cpu;
        const std::string buffer_filename = "../../build/buffer.txt";
};


#endif