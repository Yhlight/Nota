include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1
)

FetchContent_MakeAvailable(googletest)

add_executable(nota-tests TestLexer.cpp TestParser.cpp)

target_link_libraries(
    nota-tests
    GTest::gtest_main
    nota_core
)

add_test(NAME nota-tests COMMAND nota-tests)
