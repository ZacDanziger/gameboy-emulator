#include "mmu.h"

 
void MMU::init(Timer* timer_ptr, PPU* ppu_ptr) {
    timer = timer_ptr;
    ppu = ppu_ptr;
}


/**
 * Read memory[address]
 * Cannot currently read from ERAM
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

    // for tetris, remove when joypad is implemented
    if (address == JOYP_REGISTER) {
        return 0x0F;
    }

    if (address < ROM_BANK_NN_START) { return rom_bank_00[address]; }
    if (address < VRAM_START)        { return rom_bank_nn[address - ROM_BANK_NN_START]; }
    if (address < ERAM_START)        { return ppu->read(address); }
    if (address >= WRAM_BANK_00_START && address < WRAM_BANK_NN_START) { return wram_bank_00[address - WRAM_BANK_00_START]; }
    if (address < ECHO_START)        { return wram_bank_nn[address - WRAM_BANK_NN_START]; }
    if (address < NOT_USABLE_START)  { return ppu->read(address); }
    if (address >= IO_START && address < HRAM_START) { return io_registers[address - IO_START]; }
    if (address < IE_REGISTER)       { return hram[address - HRAM_START]; }
    if (address == IE_REGISTER)      { return ie_register; }
    throw std::runtime_error("Cannot read from that area of memory");
}


/**
 * Write to memory[address]
 * Cannot write to ROM, ERAM
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

        if (address == DMA_REGISTER) {
            oam_dma_transfer(data);
        }
        return;
    }

    if (address >= ECHO_START && address < OAM_START) {
        address -= 0x2000;
    }

    if (address >= VRAM_START && address < ERAM_START) {
        ppu->write(address, data);
        return;
    }
    if (address >= WRAM_BANK_00_START && address < WRAM_BANK_NN_START) {
        wram_bank_00[address - WRAM_BANK_00_START] = data;
        return;
    }
    if (address < ECHO_START) {
        wram_bank_nn[address - WRAM_BANK_NN_START] = data;
        return;
    }
    if (address < NOT_USABLE_START) {
        ppu->write(address, data);
        return;
    }
    if (address >= IO_START && address < HRAM_START) {
        io_registers[address - IO_START] = data;
        return;
    }
    if (address < IE_REGISTER) {
        hram[address - HRAM_START] = data;
        return;
    }
    if (address == IE_REGISTER) {
        ie_register = data;
        return;
    }

    throw std::runtime_error("Cannot write to that area of memory");
}


/**
 * Writes the data from a file to one of the sections of memory.
 * If the size of the data in bytes is larger than the size of the array only the first
 *     n bytes are read in, where n = the size of the array
 * --- NEW VERSION NEEDS TESTING ---
 * 
 * @param address 16-bit address that needs to be one of the starting addresses defined in memory_map.h
 * @param filename the filename containing the data to be read from
*/
void MMU::load(Address address, const std::string& filename) {
    std::vector<Byte> data = read_file(filename);
    size_t size = 0;
    

    switch(address) {
    case ROM_BANK_00_START:
        size = std::min(data.size(), (size_t)ROM_BANK_SIZE);
        std::copy_n(data.begin(), size, rom_bank_00.begin());
        break;
    case ROM_BANK_NN_START:
        size = std::min(data.size(), (size_t)ROM_BANK_SIZE);
        std::copy_n(data.begin(), size, rom_bank_nn.begin());
        break;
    case VRAM_START:
        ppu->load(address, data);
        break;
    case ERAM_START:
        size = std::min(data.size(), (size_t)ERAM_SIZE);
        // ERAM NOT SUPPORTED YET
        break;
    case WRAM_BANK_00_START:
        size = std::min(data.size(), (size_t)WRAM_BANK_SIZE);
        std::copy_n(data.begin(), size, wram_bank_00.begin());
        break;
    case WRAM_BANK_NN_START:
        size = std::min(data.size(), (size_t)WRAM_BANK_SIZE);
        std::copy_n(data.begin(), size, wram_bank_nn.begin());
        break;
    case ECHO_START:
        // NOT GOING TO ALLOW LOADS TO ECHO RAM
        break;
    case OAM_START:
        ppu->load(address, data);
        break;
    case IO_START:
        size = std::min(data.size(), (size_t)IO_REG_SIZE);
        std::copy_n(data.begin(), size, io_registers.begin());

        timer->write(DIV_REGISTER, io_registers[DIV_REGISTER - IO_START]);
        timer->write(TIMA_REGISTER, io_registers[TIMA_REGISTER - IO_START]);
        timer->write(TMA_REGISTER, io_registers[TMA_REGISTER - IO_START]);
        timer->write(TAC_REGISTER, io_registers[TAC_REGISTER - IO_START]);

        ppu->write(LCDC_REGISTER, io_registers[LCDC_REGISTER - IO_START]);
        ppu->write(STAT_REGISTER, io_registers[STAT_REGISTER - IO_START]);
        ppu->write(SCY_REGISTER, io_registers[SCY_REGISTER - IO_START]);
        ppu->write(SCX_REGISTER, io_registers[SCX_REGISTER - IO_START]);
        ppu->write(LY_REGISTER, io_registers[LY_REGISTER - IO_START]);
        ppu->write(LYC_REGISTER, io_registers[LYC_REGISTER - IO_START]);
        ppu->write(DMA_REGISTER, io_registers[DMA_REGISTER - IO_START]);
        ppu->write(BGP_REGISTER, io_registers[BGP_REGISTER - IO_START]);
        ppu->write(OBP0_REGISTER, io_registers[OBP0_REGISTER - IO_START]);
        ppu->write(OBP1_REGISTER, io_registers[OBP1_REGISTER - IO_START]);
        ppu->write(WY_REGISTER, io_registers[WY_REGISTER - IO_START]);
        ppu->write(WX_REGISTER, io_registers[WX_REGISTER - IO_START]);
        
        break;
    case HRAM_START:
        size = std::min(data.size(), (size_t)HRAM_SIZE);
        std::copy_n(data.begin(), size, hram.begin());
        break;
    default:
        throw std::runtime_error("Unsupported load address");
    }
}


/**
 * Writes the data in a file to both ROM banks.
 * There must be more than 0x8000 bytes of data in the file or an error will be thrown
 * 
 * @param filename the filename containing the data to be read from
*/
void MMU::load_rom(const std::string& filename) {
    std::vector<Byte> data = read_file(filename);

    if (data[MBC_TYPE] != 0x00) {
        throw std::runtime_error("Cannot currently handle any mbc type other than type 0");
    }

    // check header checksum
    Byte checksum = 0x00;
    for (Address address = TITLE_START; address < HEADER_CHECKSUM; address++) {
        checksum -= data[address] + 1;
    }

    if (checksum != data[HEADER_CHECKSUM]) {
        throw std::runtime_error("ROM header checksum failed");
    }

    std::copy_n(data.begin(), ROM_BANK_SIZE, rom_bank_00.begin());
    std::copy_n(data.begin() + ROM_BANK_SIZE, ROM_BANK_SIZE, rom_bank_nn.begin());
}


void MMU::request_interrupt(Interrupt interrupt) {
    io_registers[0x0F] |= static_cast<Byte>(interrupt);
}


void MMU::oam_dma_transfer(const Byte value) {
    Address address = static_cast<Address>(value) << 8;
    std::vector<Byte> dma_data(OAM_SIZE);

    for (int i = 0; i < OAM_SIZE; i++) {
        dma_data[i] = read(address + i);
    }
    
    ppu->load(OAM_START, dma_data);
}