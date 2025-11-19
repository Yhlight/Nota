add_library(NotaCore STATIC
    core/Chunk.cpp
    lexer/Lexer.cpp
    parser/Parser.cpp
    compiler/Compiler.cpp
    vm/VM.cpp
    debug/Disassembler.cpp
)

target_include_directories(NotaCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(cli)
