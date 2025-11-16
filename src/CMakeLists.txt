# Create the nota_core static library
add_library(nota_core STATIC
    Nota.cpp
    Nota.h
    Token.h
    Lexer.h
    Lexer.cpp
)

# Expose the src directory as a public include directory
target_include_directories(nota_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# The executable should be lean, with most logic in the library
add_executable(nota main.cpp)

# Link the executable against the core library
target_link_libraries(nota PRIVATE nota_core)
