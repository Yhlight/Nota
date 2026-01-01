# Add the test executable
add_executable(run_tests
    sample_test.cpp
    lexer_test.cpp
    ../src/Lexer.cpp  # Add Lexer implementation to the test build
)

# Add include directory for the main project headers
target_include_directories(run_tests PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Link the test executable against GoogleTest
target_link_libraries(run_tests GTest::gtest_main)

# Discover and add tests to CTest
include(GoogleTest)
gtest_discover_tests(run_tests)
