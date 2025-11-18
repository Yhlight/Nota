add_library(nota_core nota_core.cpp nota_core.h)

add_executable(nota main.cpp)
target_link_libraries(nota PRIVATE nota_core)
