add_executable(nota main.cpp)

add_subdirectory(nota_core)
add_subdirectory(nota_test)

target_link_libraries(nota PRIVATE nota_core)
