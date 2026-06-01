cmake_minimum_required(VERSION 3.15)
project(gameboy-emulator VERSION 0.1.0 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

# include_directories(src)
add_subdirectory(vendored)
add_subdirectory(src)
add_subdirectory(tests)


add_executable(gameboy-emulator main.cpp)
target_link_libraries(gameboy-emulator gameboy)