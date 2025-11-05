include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(memory_test test_memory.cpp)

target_link_libraries(memory_test
  PRIVATE
  GTest::GTest
  src
)

add_test(memory_gtests memory_test)

add_executable(cpu_test test_cpu.cpp)

target_link_libraries(cpu_test
  PRIVATE
  GTest::GTest
  src
)

add_test(cpu_gtests cpu_test)