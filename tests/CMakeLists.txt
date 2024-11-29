
enable_testing()

include_directories()

add_executable(
        tests
        common/print_tests.cpp
)

target_compile_definitions(tests PRIVATE UNIT_TEST)

target_link_libraries(
        tests
        GTest::gtest
        GTest::gtest_main
)
target_include_directories(tests PUBLIC ../src/common)

include(GoogleTest)
gtest_discover_tests(tests)

