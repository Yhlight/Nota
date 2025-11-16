add_executable(nota_tests main.test.cpp)

target_link_libraries(nota_tests PRIVATE gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(nota_tests)
