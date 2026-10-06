#include "gameboy.h"

/**
 * Reset the GameBoy and all of its members to their post Boot ROM states
 */
void GameBoy::reset() {
    interrupt.reset();
    joypad.reset();
    timer.reset();
    ppu.reset();
    apu.reset();
    bus.reset();
    cpu.reset();

    cgb_mode = false;

    double_speed_mode = false;
    speed_switch_delay_counter = 0x0000;
}


/**
 * Read data from a file and write it to ROM
 * NOTE: data in header section must pass gameboy rom checksum
 * 
 * @param filename the filename containing the data to be read from
*/
void GameBoy::load_rom(const std::string& rom_file) {
    reset();
    bus.load(rom_file);
    cgb_mode = bus.get_cgb_mode();
}


/**
 * Run the GameBoy until a frame is ready to be displayed,at which point
 *     the frame should be passed to the Frontend
 */
void GameBoy::run_until_frame() {
    while (!ppu.is_frame_ready()) {
        bool skip_cpu = false;

        // 1. Check if CPU is requesting a speed switch
        if (cpu.is_speed_switch_pending()) {
            cpu.clear_speed_switch_pending();
            speed_switch_delay_counter = SPEED_SWITCH_DELAY;
        }

        // 2. Tick speed switch delay counter if it is greater than 0, if it reaches 0, switch speed
        if (speed_switch_delay_counter > 0) {
            speed_switch_delay_counter--;

            if (speed_switch_delay_counter == 0) {
                set_double_speed(!double_speed_mode);
            }

            skip_cpu = true;
        // 3. Check if CPU is in a normal STOP state
        } else if (cpu.is_stopped()) {
            if (joypad.any_button_pressed()) {
                cpu.clear_stopped();
            } else {
                skip_cpu = true;
            }
        }

        // 4. Check if DMA is running, if so, tick the DMA delay counter and skip CPU
        if (bus.tick_dma_counter()) {
            skip_cpu = true;
        }

        // 5. Execute Hardware Ticks
        int system_ticks = double_speed_mode ? 2 : 1;
        for (int i = 0; i < system_ticks; i++) {

            if (!skip_cpu) {
                cpu.tick();
            }

            timer.tick();
            bus.tick_oam_dma();
        }
        
        apu.tick();
        ppu.tick();

        // 6. If the PPU is in HBlank mode and HDMA is active, transfer 16 bytes of data to VRAM
        if (ppu.take_hblank_event() && bus.get_hdma_active()) {
            bus.hdma_tick();
        }

        // 7. If the Timer's DIV/APU event is set, tick the APU's frame sequencer
        if (timer.take_div_apu_event()) {
            apu.frame_sequencer();
        }
    }
}


void GameBoy::set_double_speed(const bool ds) {
    double_speed_mode = ds;

    // Disarm switch
    Byte key1_register = bus.read(KEY1_SPD_REGISTER);
    reset_bit(key1_register, Bit::Bit0);
    bus.write(KEY1_SPD_REGISTER, key1_register);

    // Set or reset the speed bit in the KEY1 register
    bus.update_speed(double_speed_mode);

    // Update the Timer's DIV/APU bit to match the new speed mode
    timer.set_div_apu_bit(double_speed_mode);
}