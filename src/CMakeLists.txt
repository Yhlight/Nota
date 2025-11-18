add_library(nota_core
    Lexer.cpp
    ASTPrinter.cpp
)

target_include_directories(nota_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
