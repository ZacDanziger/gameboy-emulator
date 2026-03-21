#include "mbc3.h"

Byte MBC3::read(const Address address) const {
    uint32_t adjusted_address = 0x00000000;

    // read from bank 00
    if (address < ROM_BANK_NN_START) {
        return rom[address];
    }

    // read from bank nn
    if (address < VRAM_START) {
        adjusted_address = address - ROM_BANK_NN_START;
        adjusted_address += (rom_bank * ROM_BANK_SIZE);

        return rom[adjusted_address];
    }

    // read from eram
    if ((address >= ERAM_START) && (address < WRAM_BANK_00_START)) {
        if (!ram_enable) {
            return 0xFF;
        }

        if (secondary_bank >= 0x08) {
            return rtc_read();
        }

        if (eram.size() == 0) {
            return 0xFF;
        }

        adjusted_address = address - ERAM_START;
        adjusted_address += (secondary_bank * ERAM_BANK_SIZE);

        // if address is out of bounds, wrap around
        adjusted_address %= eram.size();

        return eram[adjusted_address];
    }

    throw std::runtime_error("MBC read called on incorrect address");
}


void MBC3::write(const Address address, const Byte data) {
    // enable RAM
    if (address < 0x2000) {
        ram_enable = ((data & 0x0F) == 0x0A);
        return;
    }

    // ROM bank number
    if (address < 0x4000) {
        rom_bank = (data & 0x7F);
        if (rom_bank == 0) {
            rom_bank = 1;
        }

        // clamp rom bank selected to number of rom banks available
        rom_bank %= num_rom_banks;
        return;
    }

    // Secondary bank number
    if (address < 0x6000) {
        if (data <= 0x0C){
            secondary_bank = data;
        }
        return;
    }

    // Latch clock data
    if (address < 0x8000) {
        if ((data == 0x01) && (latch_prev == 0x00)) {
            latch_time();
        }
        latch_prev = data;
        return;
    }

    if ((address >= ERAM_START) && (address < WRAM_BANK_00_START)) {
        if ((!ram_enable) || (eram.size() == 0)) {
            return;
        }

        // ignore RTC writes
        if (secondary_bank >= 0x08) {
            return;
        }

        uint32_t adjusted_address = address - ERAM_START;
        adjusted_address += (secondary_bank * ERAM_BANK_SIZE);

        adjusted_address %= eram.size();

        eram[adjusted_address] = data;
        return;
    }

    throw std::runtime_error("MBC write called on incorrect address");
}


void MBC3::save(const std::string& filename) const {
    if (!has_battery) {
        return;
    }

    std::string save_file = filename.substr(0, filename.find_last_of('.')) + ".sav";

    std::vector<Byte> data(eram.begin(), eram.end());

    data.push_back(latched_seconds);
    data.push_back(latched_minutes);
    data.push_back(latched_hours);
    data.push_back(latched_days_low);
    data.push_back(latched_days_high);

    // timestamp - 8 bytes
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(rtc_start_time.time_since_epoch()).count();
    for (int i = 0; i < 8; i++) {
        data.push_back((timestamp >> (i * 8)) & 0xFF);
    }

    write_file(save_file, data);

    return;
}


void MBC3::load(const std::string& filename) {
    if (!has_battery) {
        return;
    }

    std::string save_file = filename.substr(0, filename.find_last_of('.')) + ".sav";

    // confirm save file exists
    std::ifstream file(save_file);
    if (!file.is_open()) return;
    file.close();

    std::vector<Byte> data = read_file(save_file);

    size_t eram_size = eram.size();
    
    std::copy_n(data.begin(), eram_size, eram.begin());
    
    // older save files without RTC data
    if (data.size() == eram_size) {
        return;
    }

    latched_seconds = data[eram_size];
    latched_minutes = data[eram_size + 1];
    latched_hours = data[eram_size + 2];
    latched_days_low = data[eram_size + 3];
    latched_days_high = data[eram_size + 4];

    uint64_t timestamp = 0;
    for (int i = 0; i < 8; i++) {
        timestamp |= static_cast<uint64_t>(data[eram_size + 5 + i]) << (i * 8);
    }
    if (timestamp == 0) {
        rtc_start_time = std::chrono::system_clock::time_point(std::chrono::seconds(timestamp));
    }

    return;
}


Byte MBC3::rtc_read() const {
    switch(secondary_bank) {
    case 0x08:
        return latched_seconds;
    case 0x09:
        return latched_minutes;
    case 0x0A:
        return latched_hours;
    case 0x0B:
        return latched_days_low;
    case 0x0C:
        return latched_days_high;
    default:
        throw std::runtime_error("RTC read called on incorrect secondary bank value");
    }
}


void MBC3::latch_time() {
    auto now = std::chrono::system_clock::now();
    auto elapsed = now - rtc_start_time;

    uint64_t total_seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();

    // if halt flag is set, do not updated latched values
    if (is_set(latched_days_high, Bit::Bit6)) {
        return;
    }

    latched_seconds = total_seconds % 60;
    latched_minutes = (total_seconds / 60) % 60;
    latched_hours = (total_seconds / 3600) % 24;

    uint64_t total_days = total_seconds / 86400;
    latched_days_low = total_days & 0xFF;
    latched_days_high = (total_days >> 8) & 0x01;

    if (total_days > 511) {
        // set day counter carry bit
        set_bit(latched_days_high, Bit::Bit7);
    }
}