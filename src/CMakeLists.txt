# Create a static library for the core application logic
add_library(nota_lib STATIC)

# Explicitly list all source files for the library
target_sources(nota_lib PRIVATE
    Nota.cpp
    Lexer.cpp
    Parser.cpp
)

# Make library headers available to consumers
target_include_directories(nota_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
