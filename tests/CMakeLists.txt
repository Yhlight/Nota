# Add the test executable
add_executable(run_tests sample_test.cpp)

# Link the test executable against GoogleTest
target_link_libraries(run_tests GTest::gtest_main)

# Discover and add tests to CTest
include(GoogleTest)
gtest_discover_tests(run_tests)
