#include "mmu.h"
#include "../mbc/mbc0.h"
#include "../mbc/mbc1.h"
#include "../mbc/mbc3.h"

 
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
    if (address < ECHO_START) {
        // read from bank 0
        if (address < WRAM_BANK_NN_START) {
            return wram[address - WRAM_BANK_00_START];
        }

        // read from switchable bank 1-7
        uint32_t adjusted_address = static_cast<uint32_t>(address);
        if (cgb_mode) {
            adjusted_address += wram_bank * WRAM_BANK_SIZE;
        }
        return wram[adjusted_address - WRAM_BANK_00_START];
    }

    // OAM read
    if (address < NOT_USABLE_START) {
        return ppu->read(address);
    }

    // NOT USABLE read
    if (address < IO_START) {
        throw std::runtime_error("MMU read called on NOT USABLE section of memory (0xFEA0 - 0xFEFF)");
    }

    // IO registers read
    if (address < HRAM_START) {
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
        if (address == HDMA1_REGISTER) {
            return cgb_mode ? vram_source_high : 0xFF;
        }
        if (address == HDMA2_REGISTER) {
            return cgb_mode ? vram_source_low : 0xFF;
        }
        if (address == HDMA3_REGISTER) {
            return cgb_mode ? vram_dest_high : 0xFF;
        }
        if (address == HDMA4_REGISTER) {
            return cgb_mode ? vram_dest_low : 0xFF;
        }
        if (address == HDMA5_REGISTER) {
            return cgb_mode ? vram_dma_control : 0xFF;
        }
        if (address == SVBK_WBK_REGISTER) {
            return cgb_mode ? wram_bank : 0xFF;
        }

        // Timer IO registers
        if (address >= DIV_REGISTER && address <= TAC_REGISTER) {
            return timer->read(address);
        }

        // TODO: APU IO registers here

        if (address == KEY1_SPD_REGISTER) {
            if (timer->get_double_speed()) {
                return 0x80 | prep_speed_switch;
            } else {
                return prep_speed_switch;
            }
        }

        // PPU IO registers
        if ((address >= LCDC_REGISTER && address <= WX_REGISTER) ||
            (address == VBK_REGISTER) ||
            (address >= BCPS_BGPI_REGISTER && address <= OPRI_REGISTER))
        {
            return ppu->read(address);
        }

        return io_registers[address - IO_START];
    }
    if (address < IE_REGISTER)       { return hram[address - HRAM_START]; }
    if (address == IE_REGISTER)      { return ie_register; }

    throw std::runtime_error("MMU read called on invalid address");
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
    if (address < ECHO_START) {
        // bank 0 write
        if (address < WRAM_BANK_NN_START) {
            wram[address - WRAM_BANK_00_START] = data;
            return;
        }

        // switchable bank 1-7 write
        uint32_t adjusted_address = static_cast<uint32_t>(address);
        if (cgb_mode) {
            adjusted_address += wram_bank * WRAM_BANK_SIZE;
        }
        wram[adjusted_address - WRAM_BANK_00_START] = data;
        return;
    }

    // OAM write
    if (address < NOT_USABLE_START) {
        ppu->write(address, data);
        return;
    }

    // NOT USABLE write
    if (address < IO_START) {
        throw std::runtime_error("MMU write called on NOT USABLE section of memory (0xFEA0 - 0xFEFF)");
    }

    // IO registers write
    if (address < HRAM_START) {
        if (address == JOYP_REGISTER) {
            // lower nibble of JOYP is read-only
            data &= 0xF0;
        }

        if (address == HDMA1_REGISTER) {
            vram_source_high = data;
            return;
        }
        if (address == HDMA2_REGISTER) {
            vram_source_low = data;
            return;
        }
        if (address == HDMA3_REGISTER) {
            vram_dest_high = data;
            return;
        }
        if (address == HDMA4_REGISTER) {
            vram_dest_low = data;
            return;
        }
        if (address == HDMA5_REGISTER) {
            vram_dma_control = data;
            
            // Manually terminate HBlank transfer
            if (hdma_state.active && !is_set(data, Bit::Bit7)) {
                hdma_state.active = false;
                set_bit(vram_dma_control, Bit::Bit7);
                return;
            }
            vram_dma_transfer(data);
            return;
        }
        if (address == SVBK_WBK_REGISTER) {
            wram_bank = data & 0x07;
            if (wram_bank == 0) {
                wram_bank = 1;
            }
            return;
        } 

        // Timer IO registers
        if (address >= DIV_REGISTER && address <= TAC_REGISTER) {
            timer->write(address, data);
            return;
        }

        // TODO: APU IO registers here

        if (address == KEY1_SPD_REGISTER) {
            prep_speed_switch = data & 0x01;
            return;
        }
        // PPU IO registers
        if ((address >= LCDC_REGISTER && address <= WX_REGISTER) ||
            (address == VBK_REGISTER) ||
            (address >= BCPS_BGPI_REGISTER && address <= OPRI_REGISTER)) 
        {

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

    throw std::runtime_error("MMU write called on invalid address");
}


/**
 * Writes the data in a file to both ROM banks.
 * There must be more than 0x8000 bytes of data in the file or an error will be thrown
 * 
 * @param filename the filename containing the data to be read from
*/
void MMU::load_rom(const std::string& filename) {
    rom_filename = filename;

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
    cgb_mode = (data[CGB_FLAG] == 0x80) || (data[CGB_FLAG] == 0xC0);
    ppu->set_cgb_mode(cgb_mode);

    size_t ram_size_bytes = 0;
    switch(ram_size) {
    case 0x00:
        ram_size_bytes = 0;
        break;
    case 0x02:
        ram_size_bytes = ERAM_BANK_SIZE;
        break;
    case 0x03:
        ram_size_bytes = 4 * ERAM_BANK_SIZE;
        break;
    case 0x04:
        ram_size_bytes = 16 * ERAM_BANK_SIZE;
        break;
    case 0x05:
        ram_size_bytes = 8 * ERAM_BANK_SIZE;
        break;
    default:
        throw std::runtime_error("Unsupported RAM size type");
    }

    switch (mbc_type) {
    case 0x00:
        mbc = std::make_unique<MBC0>(std::move(data), ram_size_bytes);
        break;
    case 0x01:
        mbc = std::make_unique<MBC1>(std::move(data), 0, false);
        break;
    case 0x02:
        mbc = std::make_unique<MBC1>(std::move(data), ram_size_bytes, false);
        break;
    case 0x03:
        mbc = std::make_unique<MBC1>(std::move(data), ram_size_bytes, true);
        break;
    case 0x0F:
        mbc = std::make_unique<MBC3>(std::move(data), 0, true);
        break;
    case 0x11:
        mbc = std::make_unique<MBC3>(std::move(data), 0, false);
        break;
    case 0x12:
        mbc = std::make_unique<MBC3>(std::move(data), ram_size_bytes, false);
        break;
    case 0x10:
    case 0x13:
        mbc = std::make_unique<MBC3>(std::move(data), ram_size_bytes, true);
        break;
    default:
        throw std::runtime_error("Unsupported MBC type");
    }

    mbc->load(rom_filename);
}


void MMU::request_interrupt(Interrupt interrupt) {
    io_registers[IF_REGISTER - IO_START] |= static_cast<Byte>(interrupt);
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


void MMU::vram_dma_transfer(const Byte value) {
    if (!cgb_mode) {
        return;
    }

    bool is_hdma = is_set(value, Bit::Bit7);
    Byte len = value & 0x7F;
    Word length = (static_cast<Word>(len) + 1) << 4;

    Address source_address = (vram_source_high << 8) | (vram_source_low & 0xF0);
    Address destination_address = VRAM_START | ((vram_dest_high & 0x1F) << 8) | (vram_dest_low & 0xF0);



    if (is_hdma) {
        // HBlank DMA
        reset_bit(vram_dma_control, Bit::Bit7);
        hdma_state.active = true;
        hdma_state.source = source_address;
        hdma_state.destination = destination_address;
        hdma_state.remaining = length;
    } else {
        // General Purpose DMA
        std::vector<Byte> dma_data(length);
        for (int i = 0; i < length; i++) {
            dma_data[i] = read(source_address + i);
        }
        ppu->load(destination_address, dma_data);

        // signal transfer is complete
        vram_dma_control = 0xFF;
    }
}


void MMU::hdma_tick() {
    if (!hdma_state.active) {
        return;
    }


    if (hdma_state.remaining == 0) {
        hdma_state.active = false;

        //signal transfer is complete
        vram_dma_control = 0xFF;
        return;
    }

    int chunk_size = 16;

    std::vector<Byte> chunk(chunk_size);

    for (int i = 0; i < chunk_size; i++) {
        chunk[i] = read(hdma_state.source + i);
    }

    ppu->load(hdma_state.destination, chunk);

    hdma_state.source += chunk_size;
    hdma_state.destination += chunk_size;
    hdma_state.remaining -= chunk_size;

    Byte new_val = (hdma_state.remaining >> 4) - 1;
    vram_dma_control &= 0x80;
    vram_dma_control |= (new_val & 0x7F);
}