enable_testing()

add_subdirectory(lib/googletest)

add_executable(nota_tests main.cpp)

target_link_libraries(nota_tests GTest::gtest_main)

include_directories(../src)

add_test(NAME nota_tests COMMAND nota_tests)
