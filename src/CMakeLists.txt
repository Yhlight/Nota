add_library(libnota STATIC
    lib/nota.cpp
    lib/Lexer.cpp
    lib/Lexer.h
    lib/Token.h
    lib/Parser.cpp
    lib/Parser.h
    lib/Expr.h
    lib/Value.cpp
    lib/Value.h
    lib/Chunk.cpp
    lib/Chunk.h
    lib/VM.cpp
    lib/VM.h
    lib/Compiler.cpp
    lib/Compiler.h
    lib/Stmt.h
    lib/SymbolTable.cpp
    lib/SymbolTable.h
    lib/Object.cpp
    lib/Object.h
)

add_executable(nota
    main.cpp
)

target_link_libraries(nota libnota)
