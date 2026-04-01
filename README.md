# GameBoy Emulator v1
My GameBoy Color project to keep from getting rusty on my leave of absence

## Syncing
* Emulator steps the CPU
* CPU step drives the Timer
* Timer drives the PPU and (later)APU

## TODO:
* Update comments above functions reflecting any changes
* Clean up #includes on .h files
* Extract Joypad handling from MMU
* CPU
    * Implement HALT bug
    * Debug double speed mode
* Timer 
    * Debug double speed mode
* MMU
    * Debug double speed mode
* MBC
    * Implement more MBC types
* PPU
    * Implement DMG compatibility palettes
* APU 
    * Implement

    

## Things I Had to Learn to Use
* CMake
    * CTest
    * GTest
* SDL3