add_executable(run_tests
    main.cpp
    LexerTest.cpp
    LexerBasicsTest.cpp
    LexerOperatorsTest.cpp
    ParserTest.cpp
    LambdaParserTest.cpp
    InterpreterTest.cpp
    ArrayTest.cpp
    TestHelper.cpp
    ClassTest.cpp
    StaticArrayTest.cpp
    ModuleTest.cpp
    PackageTest.cpp
    CompilerTest.cpp
    VMTest.cpp
)

target_link_libraries(run_tests nota gtest_main)
target_include_directories(run_tests PUBLIC ../include)

include(GoogleTest)
gtest_discover_tests(run_tests)
