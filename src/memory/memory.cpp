#include "memory.h"

Memory::Memory(size_t size) : _size(size), _bytes(_size, 0) {}

Memory::Memory(const std::vector<uint8_t>& data) : _size(data.size()), _bytes(data) {}

uint8_t Memory::read(uint16_t address) {
    return _bytes[address]; //check for edge cases
}

void Memory::write(uint16_t address, uint8_t data) {
    _bytes[address] = data; // check for edge cases
}

ROM::ROM() : Memory(0) {}

ROM::ROM(const std::vector<uint8_t>& data) : Memory(data) {}

void ROM::write(uint16_t address, uint8_t data) {
    throw std::runtime_error("Cannot write to ROM");
}

void ROM::load(char* file) {
    std::ifstream is(file, std::ios::binary);
    std::istream_iterator<uint8_t> start(is), end;
    std::vector<uint8_t> buffer(start, end);

    _size = buffer.size();
    _bytes = buffer;
}