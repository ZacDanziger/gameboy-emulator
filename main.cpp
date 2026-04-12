#include "src/emulator/emulator.h"

int main() {
    Emulator emulator;
    emulator.load("/Users/zacdanziger/Documents/Personal/Coding/gameboy-emulator/game-roms/Pokemon Crystal.gbc");

    emulator.run();
    
    return 0;
}