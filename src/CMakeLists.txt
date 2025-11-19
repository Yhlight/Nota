add_library(NotaCore STATIC
    lexer/Lexer.cpp
)

target_include_directories(NotaCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(cli)
