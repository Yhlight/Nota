enable_testing()

add_subdirectory(../third_party/googletest ${CMAKE_BINARY_DIR}/googletest)

add_executable(nota-tests
    InitialTest.cpp
    LexerTest.cpp
    ParserTest.cpp
    ResolverTest.cpp
    CodeGenTest.cpp
        ExpressionTest.cpp
)

target_link_libraries(nota-tests
    PRIVATE
    nota
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(nota-tests)
