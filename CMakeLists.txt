cmake_minimum_required(VERSION 4.1.0)
project(gameboy-emulator VERSION 0.1.0 LANGUAGES CXX)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

# include_directories(src)
add_subdirectory(src)
add_subdirectory(tests)


add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} src)
target_link_libraries(${PROJECT_NAME} gtest)