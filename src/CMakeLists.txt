add_library(nota STATIC
    Nota.cpp
    Lexer.cpp
    Parser.cpp
    Resolver.cpp
    CodeGenerator.cpp
)

target_include_directories(nota PUBLIC ../include)
