cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0048 NEW)

project(gbemu)

set (CMAKE_CXX_STANDARD 17)

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(lib/utils)
add_subdirectory(lib/fmt EXCLUDE_FROM_ALL)
add_subdirectory(lib/spdlog EXCLUDE_FROM_ALL)
add_subdirectory(lib/nlohmann_json)

add_executable(disassembler src/disassembler.cpp ${UTILS_HEADER})
add_dependencies(disassembler utils)
target_include_directories(disassembler PRIVATE ${UTILS_DIR})
target_link_libraries(disassembler utils)

add_executable(emulator src/cpu.cpp src/mmu.cpp src/emulator.cpp ${UTILS_HEADER} include/cpu.h include/mmu.h)
add_dependencies(emulator utils spdlog::spdlog)
target_include_directories(emulator PRIVATE ${UTILS_DIR} include)
target_link_libraries(emulator utils spdlog::spdlog)
