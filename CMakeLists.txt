cmake_minimum_required(VERSION 2.8)

project(gbemu)

if (DEBUG_BUILD)
    set(CMAKE_BUILD_TYPE Debug)
endif ()

add_subdirectory(lib/utils)
add_subdirectory(lib/fmt EXCLUDE_FROM_ALL)

add_executable(disassembler src/main.cpp ${UTILS_HEADER})
add_dependencies(disassembler utils)
target_include_directories(disassembler PRIVATE ${UTILS_DIR})
target_link_libraries(disassembler utils)

add_executable(emulator src/cpu.cpp src/mmu.cpp ${UTILS_HEADER} include/cpu.h include/mmu.h)
add_dependencies(emulator utils)
target_include_directories(emulator PRIVATE ${UTILS_DIR} include)
target_link_libraries(emulator utils)
