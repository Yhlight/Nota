add_executable(nota_tests
    main.test.cpp
    Lexer.test.cpp
)

# Link against both the core library and GoogleTest
target_link_libraries(nota_tests PRIVATE nota_core gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(nota_tests)
