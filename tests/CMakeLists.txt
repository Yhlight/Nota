# Define the test executable
add_executable(nota-tests
    Sample.test.cpp
)

# Link the test executable against our library and GoogleTest
target_link_libraries(nota-tests PRIVATE nota_lib gtest_main)
