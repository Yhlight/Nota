include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(nota-tests test_basic.cpp)
target_link_libraries(nota-tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(nota-tests)
