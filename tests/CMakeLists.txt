enable_testing()

add_subdirectory(lib/googletest)

add_executable(nota_tests main.cpp lexer_test.cpp ../src/lexer.cpp)

target_include_directories(nota_tests PRIVATE ../src)

target_link_libraries(nota_tests GTest::gtest_main)

add_test(NAME nota_tests COMMAND nota_tests)
