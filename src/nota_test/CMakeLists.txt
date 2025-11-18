include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG    release-1.12.1
)

FetchContent_MakeAvailable(googletest)

add_executable(nota_test main.cpp)

target_link_libraries(nota_test PRIVATE nota_core gtest_main)

include(GoogleTest)
gtest_discover_tests(nota_test)
