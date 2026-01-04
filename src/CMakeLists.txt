# Create a static library for the core application logic
add_library(nota_lib STATIC)

# Automatically gather all .cpp files in the current directory
file(GLOB nota_sources "*.cpp")
target_sources(nota_lib PRIVATE ${nota_sources})

# Make library headers available to consumers
target_include_directories(nota_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
