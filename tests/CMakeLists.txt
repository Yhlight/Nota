add_executable(nota_tests main.cpp catch_amalgamated.cpp)

target_include_directories(nota_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(nota_tests PRIVATE nota)
