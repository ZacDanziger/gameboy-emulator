# GameBoy Emulator
My GameBoy Color Emulator project to keep from getting rusty on my leave of absence. Built in C++ using SDL3 to handle frontend.

# Motivation
When I was a kid I really enjoyed playing on my GameBoy Color my parents bought me for Christmas, but I haven't touched it in years at this point. Going on my leave of absense I recognized that I needed a project to keep myself coding, and I thought "What better than going back to the GameBoy?" So, I dug into the Pan Docs (https://gbdev.io/pandocs/), read through emulator dev forums, and found write ups that other people have done on their gameboy emulators until I understood enough that I could begin. I started with memory and the CPU, eventually adding the Timer, the PPU, the SDL3 Frontend, the overarching Emulator class, finally finishing with the APU . All in all, it's been a lot of fun and while I thought I would call it once I had a fully functional GameBoy Color, I'm thinking very seriously about pushing myself and making a GameBoy Advance Emulator after this.


## TODO:
* Make emulator a stand-alone app for MacOS
* Allow loading new game ROMs while emulator is running
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