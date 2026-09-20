#include "cpu.h"
#include "../gameboy.h"

/**
 * Reset the CPU to its post Boot ROM state
 */
void CPU::reset() {
    registers.reset();
    
    clear_queue();

    current_opcode = 0x00;

    source_byte_ptr = nullptr;
    dest_byte_ptr = nullptr;
    source_addr_ptr = nullptr;
    dest_addr_ptr = nullptr;

    scratch_register = {0x0000};

    interrupts_enabled = false;
    ei_pending = false;
    halted = false;
    stopped = false;
    speed_switch_halt = false;
}

/**
 * Executes 1 Machine Cycle (m-cycle) which is exactly 4 T-states
 * 
 * @param bus a reference to the owning GameBoy for memory accesses
 */
void CPU::tick(GameBoy& bus) {
    // 1. We have reached the end of the previous instruction
    if (queue_index >= queue_size) {
        clear_queue();

        // 2. Check for Interrupts
        if (interrupt.interrupt_pending()) {
            // 2A. If Halted, wake up from Halt
            halted = false;

            // 2B. If IME == 1
            if (interrupts_enabled) {
                // 2C. Queue Interrupt
                interrupts_enabled = false;
                Byte interrupt_vector = interrupt.acknowledge_interrupt();
                queue_interrupt(interrupt_vector);
            }
        }

        // 3. Process Delayed EI
        if (ei_pending) {
            interrupts_enabled = true;
            ei_pending = false;
        }

        // 4. Fetch & Decode (If we didn't just queue an interrupt)
        if (!halted && queue_size == 0) {
            // M-cycle 1
            fetch(bus);
            decode();
        } 
    } else {
        execute_microop(microcode_queue[queue_index++], bus);
    }


    // Do as many 0 m-cycle ops as possible, stop before doing an m-cycle op
    while ((queue_index < queue_size) && (!accesses_memory(microcode_queue[queue_index]))) {
        execute_microop(microcode_queue[queue_index++], bus);
    }
}

/**
 * Fetches Mem[PC++] into current_opcode
 * 
 * @param bus a reference to the owning GameBoy for memory access
 */
void CPU::fetch(GameBoy& bus) {
    current_opcode = bus.read(registers.PC++);
}


/**
 * Pushes a MicroOp onto the queue
 * 
 * @param microop the MicroOp to be pushed
 */
void CPU::push_microop(const MicroOp microop) {
    if (queue_size >= MAX_QUEUE_SIZE) {
        throw std::runtime_error("Pipeline overflow");
    }

    microcode_queue[queue_size++] = microop;
}


/**
 * read the value of the flag stored in register F
 * 
 * @param flag the flag to be read
*/
bool CPU::get_flag(const Flag flag) const {
    return is_set(registers.F, flag);
}


/**
 * update flag to new_val
 * 
 * @param flag the flag to be updated
 * @param new_val the new value of the flag
 */ 
void CPU::update_flag(const Flag flag, bool new_val) {
    if (new_val) {
        set_bit(registers.F, flag);
    } else {
        reset_bit(registers.F, flag);
    }
}



