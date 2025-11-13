#include "ppu.h"

PPU::PPU(){

}

void PPU::init(Timer* tim, Memory* mem) {
    timer = tim;
    memory = mem;
    timer->init(memory);
}