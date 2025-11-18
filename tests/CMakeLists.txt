enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(nota_tests
    main_test.cpp
    Lexer_test.cpp
    AST_test.cpp
    Parser_test.cpp
)

target_link_libraries(nota_tests
    GTest::gtest_main
    nota_core
)

include(GoogleTest)
gtest_discover_tests(nota_tests)
