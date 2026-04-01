#include "src/emulator/emulator.h"

int main() {
    Emulator emulator;
    emulator.load("../game-roms/Pokemon Crystal.gbc");

    emulator.run();
    
    return 0;
}