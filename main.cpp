#include "src/emulator/emulator.h"

int main() {
    Emulator emulator("../game-roms/Pokemon Red(Enhanced).gb");

    emulator.run();
    return 0;
}