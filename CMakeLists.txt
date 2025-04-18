cmake_minimum_required(VERSION 3.24)
project(merge-k-link-lists)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER clang++)

add_executable(${PROJECT_NAME} ./main.cpp)
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E echo "--------------------------------------------------"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "--------------------------------------------------"
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "building and running ${PROJECT_NAME}"
)
