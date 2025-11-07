cmake_minimum_required(VERSION 4.1.0)
project(src)

set (SourceFiles
    utils/utils.cpp
    memory/memory.cpp
    cpu/cpu.cpp
    cpu/optables.cpp
)

include_directories(utils)
include_directories(memory)
include_directories(cpu)

add_library(${PROJECT_NAME} ${SourceFiles})