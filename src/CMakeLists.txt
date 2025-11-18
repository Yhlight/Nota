# nota_core library
add_library(nota_core
    nota_core.cpp
    lexer/Tokenizer.cpp
    ast/PrettyPrinter.cpp
    parser/Parser.cpp
    vm/Chunk.cpp
    vm/Disassembler.cpp
)

# Public include directories
target_include_directories(nota_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/common
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer
        ${CMAKE_CURRENT_SOURCE_DIR}/ast
        ${CMAKE_CURRENT_SOURCE_DIR}/parser
        ${CMAKE_CURRENT_SOURCE_DIR}/vm
)
