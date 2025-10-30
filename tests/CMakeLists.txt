include(FetchContent)

FetchContent_Declare(
  googletest
#   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
#   URL https://github.com/google/googletest/archive/4fe3307fb2d9f86d19777c7eb0e4809e9694dde7.zip
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(test_memory test_memory.cpp)

target_link_libraries(test_memory
    PRIVATE
    GTest::GTest
    src)