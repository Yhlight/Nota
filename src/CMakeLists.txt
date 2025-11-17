# Define the library
add_library(nota
    nota.cpp
    Lexer.cpp
    Parser.cpp
    AstPrinter.cpp
    Environment.cpp
    Interpreter.cpp
    NotaFunction.cpp
    Return.cpp
)

# Link the include directory to the library
target_include_directories(nota PUBLIC ../include)

# Define the executable
add_executable(nota_cli nota_main.cpp)

# Link the library to the executable
target_link_libraries(nota_cli nota)
