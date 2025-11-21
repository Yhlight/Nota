enable_testing()

add_executable(nota_tests
    test_main.cpp
    test_lexer.cpp
    test_compiler.cpp
    test_parser.cpp
    test_statements.cpp
    test_functions.cpp
)

target_link_libraries(nota_tests libnota)

include(CTest)
add_test(NAME nota_tests COMMAND nota_tests)
