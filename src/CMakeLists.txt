set (SourceFiles
    utils/utils.cpp
    interrupt/interrupt_controller.cpp
    joypad/joypad.cpp
    memory/memory_bus.cpp
    timer/timer.cpp
    cpu/cpu.cpp
    cpu/optables.cpp
    ppu/ppu.cpp
    frontend/frontend.cpp
    mbc/mbc0.cpp
    mbc/mbc1.cpp
    mbc/mbc3.cpp
    emulator/emulator.cpp
)


add_library(gameboy ${SourceFiles})
target_include_directories(gameboy PUBLIC
    utils interrupt joypad memory mbc timer cpu ppu apu frontend emulator 
)
target_link_libraries(gameboy vendored)