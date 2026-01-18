include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(nota-tests
  test_basic.cpp
  test_lexer.cpp
  test_parser.cpp
  test_generator.cpp
)
target_link_libraries(nota-tests
  GTest::gtest_main
  nota-core
)

include(GoogleTest)
gtest_discover_tests(nota-tests)
