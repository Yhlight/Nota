add_executable(nota_tests vm_test.cpp)

# Link against the nota library to get access to the lexer, parser, and compiler
target_link_libraries(nota_tests nota_lib)
target_link_libraries(nota_tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(nota_tests)
