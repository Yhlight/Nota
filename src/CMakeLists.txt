# Create a library for the compiler's core components
add_library(nota-core
  Lexer.cpp
  Parser.cpp
  Generator.cpp
)

# Make the include directory public for any target that links against nota-core
target_include_directories(nota-core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# The main executable depends on the core library
add_executable(nota-compiler main.cpp)
target_link_libraries(nota-compiler PRIVATE nota-core)
