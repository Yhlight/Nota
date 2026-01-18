add_executable(nota_tests
    test_dummy.cpp
)

target_link_libraries(nota_tests
    PRIVATE
    nota_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(nota_tests)
