# Add the nota library
add_library(libnota STATIC
    lib/nota.cpp
    lib/Lexer.cpp
)
target_include_directories(libnota PUBLIC lib)

# Add the nota executable
add_executable(nota app/main.cpp)
target_link_libraries(nota PRIVATE libnota)
