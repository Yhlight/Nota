# Add the nota library
# Headers:
# - lib/Token.h
# - lib/Lexer.h
# - lib/Parser.h
# - lib/VM.h
# - lib/AST.h
add_library(libnota STATIC
    lib/nota.cpp
    lib/Lexer.cpp
    lib/Parser.cpp
    lib/Interpreter.cpp
    lib/Environment.cpp
    lib/NotaObjects.cpp
)
target_include_directories(libnota PUBLIC lib)

# Add the nota executable
add_executable(nota app/main.cpp)
target_link_libraries(nota PRIVATE libnota)
