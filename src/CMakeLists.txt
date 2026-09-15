set (SourceFiles
    core/apu/apu.cpp
    core/apu/channels.cpp
    core/cartridge/file_io.cpp
    core/cartridge/mbc0.cpp
    core/cartridge/mbc1.cpp
    core/cartridge/mbc3.cpp
    core/cpu/cpu.cpp
    core/cpu/optables.cpp
    core/interrupt/interrupt_controller.cpp
    core/joypad/joypad.cpp
    core/memory/memory_bus.cpp
    core/ppu/ppu.cpp
    core/timer/timer.cpp
    core/emulation_core.cpp

    frontend/frontend.cpp
)


add_library(gameboy ${SourceFiles})
target_include_directories(gameboy PUBLIC
    utils interrupt joypad memory mbc timer cpu ppu apu frontend emulator 
)
target_link_libraries(gameboy vendored)