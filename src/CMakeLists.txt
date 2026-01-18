add_library(nota_core STATIC Lexer.cpp Parser.cpp)

target_include_directories(nota_core PUBLIC include)

add_executable(nota-compiler main.cpp)

target_link_libraries(nota-compiler PRIVATE nota_core)
