add_library(nota_lib
    Compiler.cpp
)

target_include_directories(nota_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(nota
    main.cpp
)

target_link_libraries(nota PRIVATE nota_lib)
