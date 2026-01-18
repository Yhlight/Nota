add_library(nota_lib
    lexer.cpp
    parser.cpp
    generator.cpp
)
target_include_directories(nota_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(nota main.cpp)
target_link_libraries(nota PRIVATE nota_lib)
