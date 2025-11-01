
cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 23)

project(repository_init)
set(EXECUTABLE repository_init)

add_executable(${EXECUTABLE} main.cpp)

set_target_properties(${EXECUTABLE} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set(SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/main_template.cpp")
set(DEST_FILE "${CMAKE_BINARY_DIR}/bin/main_template.cpp")

message(STATUS "Copying '${SOURCE_FILE}' to '${DEST_FILE}'")

if(EXISTS "${SOURCE_FILE}")
    configure_file(
        "${SOURCE_FILE}"
        "${DEST_FILE}"
        COPYONLY
    )
else()
    message(WARNING "File '${SOURCE_FILE}' does not exist! Cannot copy to build directory.")
endif()
