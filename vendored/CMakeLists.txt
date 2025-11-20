add_subdirectory(SDL)

add_library(vendored INTERFACE)
target_link_libraries(vendored INTERFACE SDL3::SDL3)