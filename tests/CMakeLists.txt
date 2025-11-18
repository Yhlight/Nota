include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(nota_tests
    test_tokenizer.cpp
    test_ast.cpp
    test_parser.cpp
    test_bytecode.cpp
    test_vm.cpp
    test_end_to_end.cpp
)

target_link_libraries(nota_tests
    PRIVATE
        nota_core
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(nota_tests)
