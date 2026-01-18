add_library(nota_lib
    Lexer.cpp
    Parser.cpp
    CodeGen.cpp
    ComponentRegistry.cpp
)

target_include_directories(nota_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(nota
    main.cpp
)

target_link_libraries(nota PRIVATE nota_lib)
