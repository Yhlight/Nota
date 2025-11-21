enable_testing()

add_executable(nota_tests
    test_main.cpp
    test_lexer.cpp
    test_parser.cpp
    test_interpreter.cpp
    test_class.cpp
    test_modules.cpp
)

target_link_libraries(nota_tests PRIVATE libnota)

target_include_directories(nota_tests PRIVATE vendor)

add_test(NAME nota_tests COMMAND nota_tests)
