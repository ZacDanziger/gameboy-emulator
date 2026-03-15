# GameBoy Emulator v1
My GameBoy Color project to keep from getting rusty on my leave of absence

## TODO:
* MMU
    * Handle I/O Registers differently (probably somewhere else, though maybe not)
* CPU
    * Implement weird STOP behavior
    * Implement HALT bug
    * Change code block in step() that deals with serial interrupt, handle that somewhere else
* PPU
    * Test background load
    * Implement Window handling
    * Implement Object handling
    * Implement screen with SDL3
* MBC
    * Start working on
    

## Things I Had to Learn to Use
* CMake
    * CTest
    * GTest
* SDL3 (In Progress)