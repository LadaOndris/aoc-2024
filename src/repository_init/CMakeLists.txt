
cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 20)

project(repository_init)

add_executable(repository_init main.cpp)


set(SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/main_template.cpp")
set(DEST_FILE "${CMAKE_BINARY_DIR}/src/repository_init/main_template.cpp")
add_custom_command(
        TARGET repository_init POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${SOURCE_FILE}" "${DEST_FILE}"
        COMMENT "Copying main_template.cpp to the build directory"
)