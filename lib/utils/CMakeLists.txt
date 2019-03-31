set(UTILS_HEADER ${CMAKE_CURRENT_LIST_DIR}/utils.h PARENT_SCOPE)
set(UTILS_DIR ${CMAKE_CURRENT_LIST_DIR} PARENT_SCOPE)

add_library(utils utils.cpp ${UTILS_HEADER})
target_link_libraries(utils fmt::fmt nlohmann_json::nlohmann_json)
