add_library(nota_core
    ast/ASTPrinter.cpp
    lexer/Lexer.cpp
    parser/Parser.cpp
)

target_include_directories(nota_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
