add_executable(run_tests
    main.cpp
    LexerTest.cpp
)

target_link_libraries(run_tests nota gtest_main)
target_include_directories(run_tests PUBLIC ../include)

include(GoogleTest)
gtest_discover_tests(run_tests)
