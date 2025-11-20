# Add test executable
add_executable(nota_tests
    test_main.cpp
    test_lexer.cpp
)

# Link against gtest
target_link_libraries(nota_tests PRIVATE
    nota_lib
    GTest::gtest_main
)

# Add test to CTest
include(GoogleTest)
gtest_discover_tests(nota_tests)
