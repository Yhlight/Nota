add_library(NotaCore INTERFACE)

target_include_directories(NotaCore INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(cli)
