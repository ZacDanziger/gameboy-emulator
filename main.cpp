#include "src/emulator/emulator.h"

int main() {
    Emulator emulator("../game-roms/Pokemon Red.gb");
    emulator.run();
    
    return 0;
}