include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(nota-tests LexerTests.cpp ../src/Lexer.cpp)
target_link_libraries(nota-tests GTest::gtest_main)

target_include_directories(nota-tests PUBLIC ../src ../src/include)

include(GoogleTest)
gtest_discover_tests(nota-tests)
