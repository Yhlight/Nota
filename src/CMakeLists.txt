add_library(nota_core
    lexer/Lexer.cpp
    ast/ASTPrinter.cpp
    parser/Parser.cpp
)

target_include_directories(nota_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/lexer"
    "${CMAKE_CURRENT_SOURCE_DIR}/ast"
    "${CMAKE_CURRENT_SOURCE_DIR}/parser"
)
