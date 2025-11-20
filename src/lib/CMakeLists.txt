add_library(libnota STATIC Lexer.cpp)

target_include_directories(libnota
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
