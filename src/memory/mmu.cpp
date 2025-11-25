#include "mmu.h"

MMU::MMU() {
    rom_bank_00 = {0};
    rom_bank_nn = {0};
    wram_bank_00 = {0};
    wram_bank_nn = {0};
    io_registers = {0};
    hram = {0};
    ie_register = 0x00;
}

 
void MMU::init(Timer* timer_ptr, PPU* ppu_ptr) {
    timer = timer_ptr;
    ppu = ppu_ptr;
}


/**
 * Read memory[address]
 * Cannot read from ERAM or OAM
 * 
 * @param address the address to be read
*/
Byte MMU::read(Address address) const {
    if (address >= DIV_REGISTER && address <= TAC_REGISTER) {
        return timer->read(address);
    }
    if (address >= LCDC_REGISTER && address <= WX_REGISTER) {
        return ppu->read(address);
    }

    if (address >= ECHO_START && address < OAM_START) {
        address -= 0x2000;
    }

    if (address < ROM_BANK_NN_START) { return rom_bank_00[address]; }
    if (address < VRAM_START)        { return rom_bank_nn[address - ROM_BANK_NN_START]; }
    if (address < ERAM_START)        { return ppu->read(address); }
    if (address >= WRAM_BANK_00_START && address < WRAM_BANK_NN_START) { return wram_bank_00[address - WRAM_BANK_00_START]; }
    if (address < ECHO_START)        { return wram_bank_nn[address - WRAM_BANK_NN_START]; }
    if (address <= OAM_STOP)         { return ppu->read(address); }
    if (address >= IO_START && address < HRAM_START) { return io_registers[address - IO_START]; }
    if (address < IE_REGISTER)       { return hram[address - HRAM_START]; }
    if (address == IE_REGISTER)      { return ie_register; }
    throw std::runtime_error("Cannot read from that area of memory");
}


/**
 * Write to memory[address]
 * Cannot write to ROM, ERAM, Echo RAM, or OAM
 * 
 * @param address the address to be written to
 * @param data the data to be written in the address
*/
void MMU::write(Address address, Byte data) {

    // NO WRITING TO ROM!!!     (I think)
    // if (address < ROM_BANK_NN_START) {
    //     _rom_bank_00[address] = data;
    //     return;
    // }
    // if (address < VRAM_START) {
    //     _rom_bank_nn[address - ROM_BANK_NN_START] = data;
    //     return;
    // }

    if (address >= DIV_REGISTER && address <= TAC_REGISTER) {
        timer->write(address, data);
        return;
    }
    if (address >= LCDC_REGISTER && address <= WX_REGISTER) {
        ppu->write(address, data);
        return;
    }

    if (address >= ECHO_START && address < OAM_START) {
        address -= 0x2000;
    }

    if (address >= VRAM_START && address < ERAM_START) { ppu->write(address, data); return; }
    if (address >= WRAM_BANK_00_START && address < WRAM_BANK_NN_START) { wram_bank_00[address - WRAM_BANK_00_START] = data; return; }
    if (address < ECHO_START) { wram_bank_nn[address - WRAM_BANK_NN_START] = data; return; }
    if (address <= OAM_STOP) { ppu->write(address, data); return; }
    if (address >= IO_START && address < HRAM_START) { io_registers[address - IO_START] = data; return; }
    if (address < IE_REGISTER) { hram[address - HRAM_START] = data; return; }
    if (address == IE_REGISTER) { ie_register = data; return; }

    throw std::runtime_error("Cannot write to that area of memory");
}


/**
 * Writes the data from a file to one of the sections of memory.
 * If the size of the data in bytes is larger than the size of the array only the first
 *     n bytes are read in, where n = the size of the array
 * 
 * @param address 16-bit address that needs to be one of the starting addresses defined in memory.h
 * @param filename the filename containing the data to be read from
*/
void MMU::load(Address address, const std::string& filename) {
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
void MMU::load_rom(const std::string& filename) {
    std::vector<Byte> data = read_file(filename);

    if (data.size() < 0x8000) {
        throw std::runtime_error("ROM too small (< 32 KB).");
    }

    std::copy_n(data.begin(), ROM_BANK_SIZE, rom_bank_00.begin());
    std::copy_n(data.begin() + ROM_BANK_SIZE, ROM_BANK_SIZE, rom_bank_nn.begin());
}

void MMU::request_interrupt(Interrupt interrupt) {
    int bit = -1;

    switch(interrupt) {
    case Interrupt::VBlank:
        bit = 0;
        break;
    case Interrupt::LCDStat:
        bit = 1;
        break;
    case Interrupt::Timer:
        bit = 2;
        break;
    case Interrupt::Serial:
        bit = 3;
        break;
    case Interrupt::Joypad:
        bit = 4;
        break;
    default:
        throw std::runtime_error("How did you get here?");
    }

    io_registers[0x0F] |= (1 << bit);
}

/**
 * Given one of the starting addresses of a memory region, return that memory region
 * 
 * @param address one of the starting addresses of valid memory regions
 * @return a pair containing a reference to the backing array of the memory region as well as its size
*/
std::pair<Byte*, size_t> MMU::resolve_region(const Address address) {
    switch(address) {
        case ROM_BANK_00_START:  { return {rom_bank_00.data(), ROM_BANK_SIZE}; }
        case ROM_BANK_NN_START:  { return {rom_bank_nn.data(), ROM_BANK_SIZE}; }
        case WRAM_BANK_00_START: { return {wram_bank_00.data(), WRAM_BANK_SIZE}; }
        case WRAM_BANK_NN_START: { return {wram_bank_nn.data(), WRAM_BANK_SIZE}; }
        case IO_START:           { return {io_registers.data(), IO_REG_SIZE}; }
        case HRAM_START:         { return {hram.data(), HRAM_SIZE}; }
        default: { throw std::runtime_error("Unsupported load address"); }
    }
}