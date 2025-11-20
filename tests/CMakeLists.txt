enable_testing()

add_executable(nota_tests
    test_main.cpp
    test_vm.cpp
    test_lexer.cpp
)

target_link_libraries(nota_tests libnota)

include(CTest)
add_test(NAME nota_tests COMMAND nota_tests)
