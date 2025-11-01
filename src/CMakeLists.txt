cmake_minimum_required(VERSION 4.1.0)
project(src)

set (SourceFiles
    cpu/cpu.cpp
    memory/memory.cpp
)

include_directories(cpu)
include_directories(memory)

add_library(${PROJECT_NAME} ${SourceFiles})

# target_include_directories(${PROJECT_NAME} PUBLIC cpu)
# target_include_directories(${PROJECT_NAME} PUBLIC memory)