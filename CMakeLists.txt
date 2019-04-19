cmake_minimum_required(VERSION 3.1.0)

cmake_policy(SET CMP0028 NEW)
cmake_policy(SET CMP0048 NEW)

project(gbemu)

set (CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_TESTING OFF CACHE INTERNAL "")

find_package(Qt5Widgets CONFIG REQUIRED)

add_subdirectory(lib/fmt EXCLUDE_FROM_ALL)
add_subdirectory(lib/spdlog EXCLUDE_FROM_ALL)

link_libraries(-lstdc++ -lm) # for Clang support

add_library(gsl INTERFACE)
target_include_directories(gsl INTERFACE lib/gsl/include)

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE lib/nlohmann_json)

add_library(utils src/utils.cpp include/utils.h)
target_include_directories(utils PRIVATE include)
target_link_libraries(utils fmt::fmt nlohmann_json)

add_executable(disassembler src/disassembler.cpp)
target_include_directories(disassembler PRIVATE include)
target_link_libraries(disassembler utils)

set(CORE_SOURCES src/cpu.cpp src/mmu.cpp src/gpu.cpp src/timer.cpp)
set(CORE_HEADERS include/cpu.h include/mmu.h include/gpu.h include/timer.h)
add_library(core ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(core PRIVATE include)
target_link_libraries(core utils spdlog gsl)

add_executable(emulator src/debugger.ui src/emulator.cpp src/debugger.cpp include/debugger.h include/emulator.h src/gui.cpp src/gui.ui include/gui.h)
target_include_directories(emulator PRIVATE include)
target_link_libraries(emulator core Qt5::Widgets)

add_library(catch INTERFACE)
target_include_directories(catch INTERFACE lib/catch/single_include/catch2)

add_executable(tests tests/cpu.cpp tests/utils.cpp tests/main.cpp)
target_include_directories(tests PRIVATE include)
target_link_libraries(tests catch core)

enable_testing()
add_test(NAME AllTests COMMAND tests)
