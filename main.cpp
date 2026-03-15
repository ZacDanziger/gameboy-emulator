#include "src/emulator/emulator.h"

int main() {
    Emulator emulator("../game-roms/tetris.gb");

    emulator.run();
    return 0;
}