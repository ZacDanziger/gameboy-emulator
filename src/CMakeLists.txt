set (SourceFiles
    utils/utils.cpp
    utils/logger.cpp
    memory/mmu.cpp
    timer/timer.cpp
    cpu/cpu.cpp
    cpu/optables.cpp
    ppu/ppu.cpp
    display/display.cpp
    emulator/emulator.cpp
)


add_library(gameboy ${SourceFiles})
target_include_directories(gameboy PUBLIC
    utils memory timer cpu ppu display emulator 
)
target_link_libraries(gameboy vendored)