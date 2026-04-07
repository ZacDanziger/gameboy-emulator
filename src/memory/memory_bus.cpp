#include "memory_bus.h"
#include "../mbc/mbc0.h"
#include "../mbc/mbc1.h"
#include "../mbc/mbc3.h"

/**
 * Reset Memory Bus to its post Boot ROM state
 */
void MemoryBus::reset() {
    wram = {0};
    hram = {0};

    cgb_mode = false;

    prep_speed_switch = 0x7E;
    vram_source_high = 0xFF;
    vram_source_low = 0xFF;
    vram_dest_high = 0xFF;
    vram_dest_low = 0xFF;
    vram_dma_control = 0xFF;
    wram_bank = 0x01;

    hdma_active = false;
    hdma_source = 0x0000;
    hdma_destination = 0x0000;
    hdma_remaining = 0x0000;
}

/**
 * Read data from a file and write it to ROM
 * NOTE: data in header section must pass gameboy rom checksum
 * 
 * @param filename the filename containing the data to be read from
*/
void MemoryBus::load_rom(const std::string& filename) {
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

    MBC_Type mbc_type = static_cast<MBC_Type>(data[MBC_TYPE]);
    Byte ram_size = data[CART_RAM_SIZE];
    cgb_mode = (data[CGB_FLAG] == 0x80) || (data[CGB_FLAG] == 0xC0);
    ppu.set_cgb_mode(cgb_mode);

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
    case MBC_Type::MBC0:
        mbc = std::make_unique<MBC0>(std::move(data), ram_size_bytes);
        break;
    case MBC_Type::MBC1:
        mbc = std::make_unique<MBC1>(std::move(data), 0, false);
        break;
    case MBC_Type::MBC1_RAM:
        mbc = std::make_unique<MBC1>(std::move(data), ram_size_bytes, false);
        break;
    case MBC_Type::MBC1_RAM_BATTERY:
        mbc = std::make_unique<MBC1>(std::move(data), ram_size_bytes, true);
        break;
    case MBC_Type::MBC3_TIMER_BATTERY:
        mbc = std::make_unique<MBC3>(std::move(data), 0, true);
        break;
    case MBC_Type::MBC3:
        mbc = std::make_unique<MBC3>(std::move(data), 0, false);
        break;
    case MBC_Type::MBC3_RAM:
        mbc = std::make_unique<MBC3>(std::move(data), ram_size_bytes, false);
        break;
    case MBC_Type::MBC3_TIMER_RAM_BATTERY:
    case MBC_Type::MBC3_RAM_BATTERY:
        mbc = std::make_unique<MBC3>(std::move(data), ram_size_bytes, true);
        break;
    default:
        throw std::runtime_error("Unsupported MBC type");
    }

    mbc->load(rom_filename);
}


/**
 * Read from memory
 * NOTE: reads on memory locations [0xFEA0, 0xFEFF] will return the open bus value (0xFF)
 * 
 * @param address the address to be read from
 * @returns the value in memory at address
*/
Byte MemoryBus::read(Address address) const {
    // ROM read
    if (address < VRAM_START) {
        return mbc->read(address);
    }

    // VRAM read
    if (address < ERAM_START) {
        return ppu.read(address);
    }

    // ERAM read
    if (address < WRAM_BANK_00_START) {
        return mbc->read(address);
    }

    // Echo RAM adjust (maps to WRAM)
    if (address >= ECHO_START && address < OAM_START) {
        address -= (ECHO_START - WRAM_BANK_00_START);
    }  

    // WRAM read
    if (address < ECHO_START) {
        // read from bank 0
        if (address < WRAM_BANK_NN_START) {
            return wram[address - WRAM_BANK_00_START];
        }

        // read from bank 1 (DMG) or switchable banks 1-7 (CGB)
        uint32_t adjusted_address = static_cast<uint32_t>(address);
        if (cgb_mode) {
            adjusted_address += wram_bank * WRAM_BANK_SIZE;
        } else {
            adjusted_address += WRAM_BANK_SIZE;
        }
        return wram[adjusted_address - WRAM_BANK_NN_START];
    }

    // OAM read
    if (address < NOT_USABLE_START) {
        return ppu.read(address);
    }

    // NOT USABLE read
    if (address < IO_START) {
        return OPEN_BUS_VALUE;
    }

    // IO registers read
    if (address < HRAM_START) {
        if (address == JOYP_REGISTER) {
            return joypad.read();
        }

        // CGB registers - return open bus value if in DMG mode
        if (address == HDMA1_REGISTER) {
            return cgb_mode ? vram_source_high : OPEN_BUS_VALUE;
        }
        if (address == HDMA2_REGISTER) {
            return cgb_mode ? vram_source_low : OPEN_BUS_VALUE;
        }
        if (address == HDMA3_REGISTER) {
            return cgb_mode ? vram_dest_high : OPEN_BUS_VALUE;
        }
        if (address == HDMA4_REGISTER) {
            return cgb_mode ? vram_dest_low : OPEN_BUS_VALUE;
        }
        if (address == HDMA5_REGISTER) {
            return cgb_mode ? vram_dma_control : OPEN_BUS_VALUE;
        }
        if (address == SVBK_WBK_REGISTER) {
            return cgb_mode ? wram_bank : OPEN_BUS_VALUE;
        }

        // Timer IO registers
        if (address >= DIV_REGISTER && address <= TAC_REGISTER) {
            return timer.read(address);
        }

        if (address == IF_REGISTER) {
            return interrupt.read(address);
        }
        // TODO: APU IO registers here

        if (address == KEY1_SPD_REGISTER) {
            if (timer.get_double_speed()) {
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
            return ppu.read(address);
        }

        // any IO registers not implemented just return open bus
        return OPEN_BUS_VALUE;
    }

    // HRAM read
    if (address < IE_REGISTER)       {
        return hram[address - HRAM_START];
    }

    // IE register read
    if (address == IE_REGISTER)      {
        return interrupt.read(address);
    }

    // not sure how you would get here
    throw std::runtime_error("MemoryBus read called on invalid address");
}


/**
 * Write to memory
 * NOTE: writes to memory locations [0xFEA0, 0xFEFF] will do nothing
 * 
 * @param address the address to be written to
 * @param data the data to be written in the address
*/
void MemoryBus::write(Address address, Byte data) {
    // ROM write
    if (address < VRAM_START) {
        mbc->write(address, data);
        return;
    }

    // VRAM write
    if (address >= VRAM_START && address < ERAM_START) {
        ppu.write(address, data);
        return;
    }

    // ERAM write
    if (address < WRAM_BANK_00_START) {
        mbc->write(address, data);
        return;
    }

    // Echo RAM adjust (maps to WRAM)
    if (address >= ECHO_START && address < OAM_START) {
        address -= (ECHO_START - WRAM_BANK_00_START);
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
        wram[adjusted_address - WRAM_BANK_NN_START] = data;
        return;
    }

    // OAM write
    if (address < NOT_USABLE_START) {
        ppu.write(address, data);
        return;
    }

    // NOT USABLE write
    if (address < IO_START) {
        return;
    }

    // IO registers write
    if (address < HRAM_START) {
        if (address == JOYP_REGISTER) {
            joypad.write(data);
        }

        // CGB registers - writes in DMG mode are fine, but reads in DMG mode will be 0xFF
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
            if (hdma_active && !is_set(data, Bit::Bit7)) {
                hdma_active = false;
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
            timer.write(address, data);
            return;
        }
        if (address == IF_REGISTER) {
            interrupt.write(address, data);
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

            ppu.write(address, data);

            if (address == DMA_REGISTER) {
                oam_dma_transfer(data);
            }
            return;
        }

        return;
    }

    // HRAM write
    if (address < IE_REGISTER) {
        hram[address - HRAM_START] = data;
        return;
    }

    // IE register write
    if (address == IE_REGISTER) {
        interrupt.write(address, data);
        return;
    }

    // not sure how you would get here
    throw std::runtime_error("MemoryBus write called on invalid address");
}


/**
 * Transfer a section of data exactly the size of OAM to OAM
 * 
 * @param value the upper byte of the address to start transferring data from
 */
void MemoryBus::oam_dma_transfer(const Byte value) {
    Address address = static_cast<Address>(value) << 8;
    std::vector<Byte> dma_data(OAM_SIZE);

    for (int i = 0; i < OAM_SIZE; i++) {
        dma_data[i] = read(address + i);
    }
    
    ppu.load(OAM_START, dma_data);
}


/**
 * Transfer a section of data to VRAM
 * CGB only
 * 
 * @param value whether to use HBlank DMA or general DMA, as well as the length of data to be transferred
 */
void MemoryBus::vram_dma_transfer(const Byte value) {
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
        // not sure about this if block
        if (!hdma_active) {
            hdma_source = source_address;
            hdma_destination = destination_address;
        }
        hdma_active = true;
        hdma_remaining = length;
    } else {
        // General Purpose DMA
        std::vector<Byte> dma_data(length);
        for (int i = 0; i < length; i++) {
            dma_data[i] = read(source_address + i);
        }
        ppu.load(destination_address, dma_data);

        // signal transfer is complete
        vram_dma_control = 0xFF;
    }
}


/**
 * Transfer 16 bytes of data to VRAM during the PPU's HBlank mode
 * CGB only
 */
void MemoryBus::hdma_tick() {
    if (!hdma_active) {
        return;
    }

    int chunk_size = 16;

    std::vector<Byte> chunk(chunk_size);

    for (int i = 0; i < chunk_size; i++) {
        chunk[i] = read(hdma_source + i);
    }

    if (hdma_destination + chunk_size > ERAM_START) {
        hdma_active = false;
        vram_dma_control = 0xFF;
        return;
    }

    ppu.load(hdma_destination, chunk);

    hdma_source += chunk_size;
    hdma_destination += chunk_size;
    hdma_remaining -= chunk_size;

    if (hdma_remaining == 0) {
        hdma_active = false;
        vram_dma_control = 0xFF;
    } else {
        Byte new_val = (hdma_remaining >> 4) - 1;
        vram_dma_control &= 0x80;
        vram_dma_control |= (new_val & 0x7F);
    }
}