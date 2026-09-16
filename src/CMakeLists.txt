set (SourceFiles
    core/apu/apu.cpp
    core/apu/channels.cpp
    core/cartridge/mbc0.cpp
    core/cartridge/mbc1.cpp
    core/cartridge/mbc3.cpp
    core/cpu/cpu.cpp
    core/cpu/optables.cpp
    core/interrupt/interrupt_controller.cpp
    core/joypad/joypad.cpp
    core/ppu/ppu.cpp
    core/timer/timer.cpp
    core/utils/file_io.cpp
    core/gameboy.cpp

    frontend/frontend.cpp

    app/emulator.cpp
)


add_library(gameboy ${SourceFiles})
target_include_directories(gameboy PUBLIC
    core frontend app
)
target_link_libraries(gameboy vendored)