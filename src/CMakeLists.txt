cmake_minimum_required(VERSION 4.1.0)
project(src)

set (SourceFiles
    utils/utils.cpp
    utils/logger.cpp
    memory/memory.cpp
    timer/timer.cpp
    cpu/cpu.cpp
    cpu/optables.cpp
    ppu/ppu.cpp
    emulator/emulator.cpp
)

include_directories(utils)
include_directories(memory)
include_directories(timer)
include_directories(cpu)
include_directories(ppu)
include_directories(emulator)

add_library(${PROJECT_NAME} ${SourceFiles})
target_link_libraries(${PROJECT_NAME} vendored)