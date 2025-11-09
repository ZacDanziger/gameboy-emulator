#include "memory.h"

Memory::Memory() {
    _rom_bank_00 = {0};
    _rom_bank_01 = {0};
    _vram = {0};
    _wram_bank_00 = {0};
    _wram_bank_01 = {0};
    _io_registers = {0};
    _hram = {0};
    _IE_register = 0x00;
}

/**
 * Read memory[address]
 * Cannot read from ERAM, Echo RAM, or OAM
 * 
 * @param address the address to be read
*/
Byte Memory::read(Address address) const {
    if (address == 0xFF44) { return 0x90; }  // hardcoded for testing - take out later
    if (address < ROM_BANK_01_START) { return _rom_bank_00[address]; }
    if (address < VRAM_START) { return _rom_bank_01[address - ROM_BANK_01_START]; }
    if (address < ERAM_START) { return _vram[address - VRAM_START]; }
    if (address >= WRAM_BANK_00_START && address < WRAM_BANK_01_START) { return _wram_bank_00[address - WRAM_BANK_00_START]; }
    if (address < ECHO_START) { return _wram_bank_01[address - WRAM_BANK_01_START]; }
    if (address >= IO_START && address < HRAM_START) { return _io_registers[address - IO_START]; }
    if (address < IE_REGISTER) { return _hram[address - HRAM_START]; }
    if (address == IE_REGISTER) { return _IE_register; }
    throw std::runtime_error("Cannot read from that area of memory");
}

/**
 * Write to memory[address]
 * Cannot write to ROM, ERAM, Echo RAM, or OAM
 * 
 * @param address the address to be written to
 * @param data the data to be written in the address
*/
void Memory::write(Address address, Byte data) {

    // NO WRITING TO ROM!!!
    // if (address < ROM_BANK_01_START) {
    //     _rom_bank_00[address] = data;
    //     return;
    // }
    // if (address < VRAM_START) {
    //     _rom_bank_01[address - ROM_BANK_01_START] = data;
    //     return;
    // }

    if (address < ERAM_START && address >= VRAM_START) { _vram[address - VRAM_START] = data; return; }
    if (address >= WRAM_BANK_00_START && address < WRAM_BANK_01_START) { _wram_bank_00[address - WRAM_BANK_00_START] = data; return; }
    if (address < ECHO_START) { _wram_bank_01[address - WRAM_BANK_01_START] = data; return; }
    if (address >= IO_START && address < HRAM_START) { _io_registers[address - IO_START] = data; return; }
    if (address < IE_REGISTER) { _hram[address - HRAM_START] = data; return; }
    if (address == IE_REGISTER) { _IE_register = data; return; }
    throw std::runtime_error("Cannot write to that area of memory");
}

/**
 * Writes the data from a file to one of the sections of memory. 
 * 
 * @param address 16-bit address that needs to be one of the starting addresses defined in memory.h
 * @param filename the filename containing the data to be read from
*/
void Memory::load(Address address, const std::string& filename) {
    std::vector<Byte> data = read_file(filename);
    auto [backing_array, max_size] = resolve_region(address);
    size_t size = std::min(data.size(), max_size);

    std::copy_n(data.begin(), size, backing_array);
}

/**
 * Writes the data in a file to both ROM banks.
 * There must be more than 0x8000 bytes of data in the file or an error will be thrown
 * 
 * @param filename the filename containing the data to be read from
*/
void Memory::load_rom(const std::string& filename) {
    std::vector<Byte> data = read_file(filename);

    if (data.size() < 0x8000) {
        throw std::runtime_error("ROM too small (< 32 KB).");
    }

    std::copy_n(data.begin(), ROM_BANK_SIZE, _rom_bank_00.begin());
    std::copy_n(data.begin() + ROM_BANK_SIZE, ROM_BANK_SIZE, _rom_bank_01.begin());
}

/**
 * Given one of the starting addresses of a memory region, return that memory region
 * 
 * @param address one of the starting addresses of valid memory regions
 * @return a pair containing a reference to the backing array of the memory region as well as its size
*/
std::pair<Byte*, size_t> Memory::resolve_region(const Address address) {
    switch(address) {
        case ROM_BANK_00_START:  { return {_rom_bank_00.data(), ROM_BANK_SIZE}; }
        case ROM_BANK_01_START:  { return {_rom_bank_01.data(), ROM_BANK_SIZE}; }
        case VRAM_START:         { return {_vram.data(), VRAM_SIZE}; }
        case WRAM_BANK_00_START: { return {_wram_bank_00.data(), WRAM_BANK_SIZE}; }
        case WRAM_BANK_01_START: { return {_wram_bank_01.data(), WRAM_BANK_SIZE}; }
        case IO_START:           { return {_io_registers.data(), IO_REG_SIZE}; }
        case HRAM_START:         { return {_hram.data(), HRAM_SIZE}; }
        default: { throw std::runtime_error("Unsupported load address"); }
    }
}

