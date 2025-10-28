
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(TBB REQUIRED)

# Creates a binary for initializing the repository
add_subdirectory(repository_init)

# Source code shared for all days
add_subdirectory(common)

file(GLOB DAY_DIRECTORIES day*/)

foreach(DAY_DIR ${DAY_DIRECTORIES})
    get_filename_component(DAY_NAME ${DAY_DIR} NAME)
    set(BINARY_NAME ${DAY_NAME}_solution)

    file(GLOB CPP_FILES "${DAY_DIR}/*.cpp")
    add_executable(${BINARY_NAME} ${CPP_FILES})

    target_link_libraries(${BINARY_NAME} PRIVATE shared_lib TBB::tbb)

    set_target_properties(${BINARY_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    target_compile_options(${BINARY_NAME} PRIVATE -Wall -Wextra -Wno-unused -Wshadow)

    configure_file(../input_files/${DAY_NAME}.txt ${CMAKE_BINARY_DIR}/bin/${DAY_NAME}.txt COPYONLY)
endforeach()

