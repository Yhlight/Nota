# CMakeLists.txt for the main compiler executable

# 创建可执行文件
add_executable(nota-compiler main.cpp Lexer.cpp Parser.cpp Generator.cpp)

# # # （可选）如果未来我们将 Lexer/Parser/Generator 编译成一个静态库
# # add_library(notacore Lexer.cpp Parser.cpp Generator.cpp)
# # add_executable(nota-compiler main.cpp)
# # target_link_libraries(nota-compiler PRIVATE notacore)
