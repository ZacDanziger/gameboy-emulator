#include "src/emulator/emulator.h"

int main() {
    Emulator emulator("../game-roms/Pokemon Crystal.gbc");
    emulator.run();
    
    return 0;
}