cmake_minimum_required(VERSION 2.8)

project(gbemu)

set(CMAKE_BUILD_TYPE Debug)

include_directories(include lib/utils)

add_subdirectory(lib/utils)
add_subdirectory(lib/fmt EXCLUDE_FROM_ALL)

add_executable(disassembler src/main.cpp)
add_executable(emulator src/cpu.cpp)

target_link_libraries(disassembler fmt::fmt utils)
target_link_libraries(emulator utils)
