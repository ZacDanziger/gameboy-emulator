#include "mmu.h"
#include "../mbc/mbc0.h"
#include"../mbc/mbc1.h"

 
void MMU::init(Timer* timer_ptr, PPU* ppu_ptr) {
    timer = timer_ptr;
    ppu = ppu_ptr;
}


/**
 * Read memory[address]
 * 
 * @param address the address to be read
*/
Byte MMU::read(Address address) const {
    // ROM read
    if (address < VRAM_START) {
        return mbc->read(address);
    }

    // VRAM read
    if (address < ERAM_START) {
        return ppu->read(address);
    }

    // ERAM read
    if (address < WRAM_BANK_00_START) {
        return mbc->read(address);
    }

    // Echo RAM adjust (maps to WRAM)
    if (address >= ECHO_START && address < OAM_START) {
        address -= 0x2000;
    }  

    // WRAM read
    if (address < WRAM_BANK_NN_START) {
        return wram_bank_00[address - WRAM_BANK_00_START];
    }
    if (address < ECHO_START) {
        return wram_bank_nn[address - WRAM_BANK_NN_START];
    }

    // OAM read
    if (address < NOT_USABLE_START) {
        return ppu->read(address);
    }

    // IO registers read
    if (address >= IO_START && address < HRAM_START) {
        if (address == JOYP_REGISTER) {
            Byte joypad = io_registers[JOYP_REGISTER - IO_START];

            switch(joypad & 0x30) {
            case 0x00:
                return 0x0F;
            case 0x10:
                return (0x10 | button_keys);
            case 0x20:
                return (0x20 | direction_keys);
            case 0x30:
                return 0x3F;
            }
        }

        // Timer IO registers
        if (address >= DIV_REGISTER && address <= TAC_REGISTER) {
            return timer->read(address);
        }

        // TODO: APU IO registers here

        // PPU IO registers
        if (address >= LCDC_REGISTER && address <= WX_REGISTER) {
            return ppu->read(address);
        }
        return io_registers[address - IO_START];
    }
    if (address < IE_REGISTER)       { return hram[address - HRAM_START]; }
    if (address == IE_REGISTER)      { return ie_register; }
    throw std::runtime_error("Cannot read from that area of memory");
}


/**
 * Write to memory[address]
 * 
 * @param address the address to be written to
 * @param data the data to be written in the address
*/
void MMU::write(Address address, Byte data) {

    // ROM write
    if (address < VRAM_START) {
        mbc->write(address, data);
        return;
    }

    // VRAM write
    if (address >= VRAM_START && address < ERAM_START) {
        ppu->write(address, data);
        return;
    }

    // ERAM write
    if (address < WRAM_BANK_00_START) {
        mbc->write(address, data);
        return;
    }

    // Echo RAM adjust (maps to WRAM)
    if (address >= ECHO_START && address < OAM_START) {
        address -= 0x2000;
    }
    // WRAM write
    if (address >= WRAM_BANK_00_START && address < WRAM_BANK_NN_START) {
        wram_bank_00[address - WRAM_BANK_00_START] = data;
        return;
    }
    if (address < ECHO_START) {
        wram_bank_nn[address - WRAM_BANK_NN_START] = data;
        return;
    }

    // OAM write
    if (address < NOT_USABLE_START) {
        ppu->write(address, data);
        return;
    }

    // IO registers write
    if (address >= IO_START && address < HRAM_START) {
        if (address == JOYP_REGISTER) {
            // lower nibble of JOYP is read-only
            data &= 0xF0;
        }

        // Timer IO registers
        if (address >= DIV_REGISTER && address <= TAC_REGISTER) {
            timer->write(address, data);
            return;
        }

        // TODO: APU IO registers here

        // PPU IO registers
        if (address >= LCDC_REGISTER && address <= WX_REGISTER) {
            ppu->write(address, data);

            if (address == DMA_REGISTER) {
                oam_dma_transfer(data);
            }
        return;
        }

        io_registers[address - IO_START] = data;
        return;
    }

    // HRAM write
    if (address < IE_REGISTER) {
        hram[address - HRAM_START] = data;
        return;
    }

    // IE register write
    if (address == IE_REGISTER) {
        ie_register = data;
        return;
    }

    throw std::runtime_error("Cannot write to that area of memory");
}


/**
 * Writes the data in a file to both ROM banks.
 * There must be more than 0x8000 bytes of data in the file or an error will be thrown
 * 
 * @param filename the filename containing the data to be read from
*/
void MMU::load_rom(const std::string& filename) {
    std::vector<Byte> data = read_file(filename);

    // check header checksum
    Byte checksum = 0x00;
    for (Address address = TITLE_START; address < HEADER_CHECKSUM; address++) {
        checksum -= data[address] + 1;
    }

    if (checksum != data[HEADER_CHECKSUM]) {
        throw std::runtime_error("ROM header checksum failed");
    }

    Byte mbc_type = data[MBC_TYPE];
    Byte ram_size = data[CART_RAM_SIZE];

    switch (mbc_type) {
    case 0x00:
        mbc = std::make_unique<MBC0>(std::move(data), ram_size);
        break;
    case 0x01:
        mbc = std::make_unique<MBC1>(std::move(data), 0);
        break;
    case 0x02:
    case 0x03:
        mbc = std::make_unique<MBC1>(std::move(data), ram_size);
        break;
    default:
        throw std::runtime_error("Unsupported MBC type");
    }
}


void MMU::request_interrupt(Interrupt interrupt) {
    io_registers[0x0F] |= static_cast<Byte>(interrupt);
}

void MMU::set_key(Key key, bool pressed) {
    Byte* target = nullptr;
    Bit bit = Bit::Bit0;

    switch(key) {
    case Key::A:
        target = &button_keys;
        bit = Bit::Bit0;
        break;
    case Key::B:
        target = &button_keys;
        bit = Bit::Bit1;
        break;
    case Key::Select:
        target = &button_keys;
        bit = Bit::Bit2;
        break;
    case Key::Start:
        target = &button_keys;
        bit = Bit::Bit3;
        break;
    case Key::Right:
        target = &direction_keys;
        bit = Bit::Bit0;
        break;
    case Key::Left:
        target = &direction_keys;
        bit = Bit::Bit1;
        break;
    case Key::Up:
        target = &direction_keys;
        bit = Bit::Bit2;
        break;
    case Key::Down:
        target = &direction_keys;
        bit = Bit::Bit3;
        break;
    }

    if (pressed) {
        reset_bit(*target, bit);
        request_interrupt(Interrupt::Joypad);
    } else {
        set_bit(*target, bit);
    }
}


void MMU::oam_dma_transfer(const Byte value) {
    Address address = static_cast<Address>(value) << 8;
    std::vector<Byte> dma_data(OAM_SIZE);

    for (int i = 0; i < OAM_SIZE; i++) {
        dma_data[i] = read(address + i);
    }
    
    ppu->load(OAM_START, dma_data);
}