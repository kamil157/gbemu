cmake_minimum_required(VERSION 2.8)

project(gbemu)

set(HEADER_FILES utils.h cpu.h)
include_directories(include)

add_executable(disassembler src/main.cpp src/utils.cpp include/utils.h)
add_executable(emulator src/cpu.cpp src/utils.cpp include/utils.h include/cpu.h)

add_subdirectory(lib/fmt EXCLUDE_FROM_ALL)
target_link_libraries(disassembler fmt::fmt)
