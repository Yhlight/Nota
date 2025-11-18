include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG    release-1.11.0
)

FetchContent_MakeAvailable(googletest)

add_executable(nota_tests test_main.cpp)
target_link_libraries(nota_tests PRIVATE GTest::gtest_main)
target_include_directories(nota_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(nota_tests PRIVATE nota_core)

include(GoogleTest)
gtest_discover_tests(nota_tests)
