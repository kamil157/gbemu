cmake_minimum_required(VERSION 2.8)

project(gbemu)
add_executable(${PROJECT_NAME} "main.cpp")

add_subdirectory(lib/fmt EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} fmt::fmt)
