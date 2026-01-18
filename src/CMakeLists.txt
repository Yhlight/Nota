add_library(nota_lib
    lexer.cpp
    parser.cpp
)

add_executable(nota
    main.cpp
)

target_link_libraries(nota PRIVATE nota_lib)
