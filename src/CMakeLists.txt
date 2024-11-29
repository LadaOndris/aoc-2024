
# Creates a binary for initializing the repository
add_subdirectory(repository_init)

# Source code shared for all days
add_subdirectory(common)

file(GLOB DAY_DIRECTORIES day*/)

foreach(DAY_DIR ${DAY_DIRECTORIES})
    get_filename_component(DAY_NAME ${DAY_DIR} NAME)

    file(GLOB CPP_FILES "${DAY_DIR}/*.cpp")
    add_executable(${DAY_NAME}_solution ${CPP_FILES})

    target_link_libraries(${DAY_NAME}_solution PRIVATE shared_lib)

    configure_file(../input_files/${DAY_NAME}.txt ${CMAKE_CURRENT_BINARY_DIR}/${DAY_NAME}.txt COPYONLY)
endforeach()