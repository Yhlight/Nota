add_library(libnota STATIC
    nota.cpp
)

target_include_directories(libnota PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(nota main.cpp)
target_link_libraries(nota PRIVATE libnota)
