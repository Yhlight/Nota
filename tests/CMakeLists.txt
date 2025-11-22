enable_testing()

add_executable(nota_tests
    test_main.cpp
    test_lexer.cpp
    test_parser.cpp
    test_interpreter.cpp
    test_class.cpp
    test_modules.cpp
    test_gc.cpp
    test_lambda.cpp
    test_ffi.cpp
    test_stdlib.cpp
    test_array.cpp
    test_logical_operators.cpp
    test_for_each.cpp
    test_compound_assignment.cpp
    test_none.cpp
    test_bitwise_operators.cpp
    test_mutability.cpp
    test_string.cpp
    test_do_while.cpp
)

target_link_libraries(nota_tests PRIVATE libnota)

target_include_directories(nota_tests PRIVATE vendor)

add_test(NAME nota_tests COMMAND nota_tests)
