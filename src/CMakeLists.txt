set (SourceFiles
    utils/utils.cpp
    utils/logger.cpp
    memory/mmu.cpp
    timer/timer.cpp
    cpu/cpu.cpp
    cpu/optables.cpp
    ppu/ppu.cpp
    display/display.cpp
    mbc/mbc0.cpp
    mbc/mbc1.cpp
    emulator/emulator.cpp
)


add_library(gameboy ${SourceFiles})
target_include_directories(gameboy PUBLIC
    utils memory timer cpu ppu display mbc emulator 
)
target_link_libraries(gameboy vendored)