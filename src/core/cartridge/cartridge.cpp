#include "cartridge.h"

#include "mbc0.h"
#include "mbc1.h"
#include "mbc3.h"

std::unique_ptr<Cartridge> Cartridge::load_from_file(const std::string& rom_file) {
    std::vector<Byte> data = read_file(rom_file);

    // check header checksum
    Byte checksum = 0x00;
    for (Address address = TITLE_START; address < HEADER_CHECKSUM; address++) {
        checksum -= data[address] + 1;
    }

    if (checksum != data[HEADER_CHECKSUM]) {
        throw std::runtime_error("ROM header checksum failed");
    }


    MBC_Type cartridge_type = static_cast<MBC_Type>(data[MBC_TYPE]);
    Byte ram_size = data[CART_RAM_SIZE];
    
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

    std::unique_ptr<Cartridge> cartridge;
    bool is_cgb = (data[CGB_FLAG] == 0x80) || (data[CGB_FLAG] == 0xC0);
    
    switch (cartridge_type) {
    case MBC_Type::MBC0:
        cartridge = std::make_unique<MBC0>(std::move(data), ram_size_bytes);
        break;
    case MBC_Type::MBC1:
        cartridge = std::make_unique<MBC1>(std::move(data), 0, false);
        break;
    case MBC_Type::MBC1_RAM:
        cartridge = std::make_unique<MBC1>(std::move(data), ram_size_bytes, false);
        break;
    case MBC_Type::MBC1_RAM_BATTERY:
        cartridge = std::make_unique<MBC1>(std::move(data), ram_size_bytes, true);
        break;
    case MBC_Type::MBC3_TIMER_BATTERY:
        cartridge = std::make_unique<MBC3>(std::move(data), 0, true);
        break;
    case MBC_Type::MBC3:
        cartridge = std::make_unique<MBC3>(std::move(data), 0, false);
        break;
    case MBC_Type::MBC3_RAM:
        cartridge = std::make_unique<MBC3>(std::move(data), ram_size_bytes, false);
        break;
    case MBC_Type::MBC3_TIMER_RAM_BATTERY:
    case MBC_Type::MBC3_RAM_BATTERY:
        cartridge = std::make_unique<MBC3>(std::move(data), ram_size_bytes, true);
        break;
    default:
        throw std::runtime_error("Unsupported MBC type");
    }

    cartridge->load(rom_file);
    cartridge->cgb_mode = is_cgb;

    return cartridge;
}