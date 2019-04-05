cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0028 NEW)

project(gbemu)

set (CMAKE_CXX_STANDARD 17)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_TESTING OFF)

add_subdirectory(lib/utils)
add_subdirectory(lib/fmt EXCLUDE_FROM_ALL)
add_subdirectory(lib/spdlog EXCLUDE_FROM_ALL)
add_subdirectory(lib/nlohmann_json)
add_subdirectory(lib/catch)

add_executable(disassembler src/disassembler.cpp)
target_include_directories(disassembler PRIVATE ${UTILS_DIR})
target_link_libraries(disassembler utils)

set(CORE_SOURCES src/cpu.cpp src/mmu.cpp)
set(CORE_HEADERS include/cpu.h include/mmu.h)
add_library(core ${CORE_SOURCES} ${UTILS_HEADER} ${CORE_HEADERS})
target_include_directories(core PRIVATE ${UTILS_DIR} include)
target_link_libraries(core utils)

add_executable(emulator src/emulator.cpp)
target_include_directories(emulator PRIVATE ${UTILS_DIR} include)
target_link_libraries(emulator utils spdlog core)

add_library(catch INTERFACE)
target_include_directories(catch INTERFACE lib/catch/single_include/catch2)

add_executable(tests tests/cpu.cpp ${UTILS_HEADER})
target_include_directories(tests PRIVATE ${UTILS_DIR} include)
target_link_libraries(tests catch utils core)

enable_testing()
add_test(NAME AllTests COMMAND tests)
