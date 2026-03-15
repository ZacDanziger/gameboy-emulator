#include "src/emulator/emulator.h"

int main() {
    Emulator emulator("../game-roms/tetris.gb");

    emulator.run();

    // dump("../game-roms/tetris.gb");

    return 0;
}