#include "../src/Lexer.h"
#include <cassert>
#include <iostream>

void test_single_line_comment() {
    std::cout << "Running test: test_single_line_comment..." << std::endl;
    std::string source = "// this is a comment\n"
                         "// this is another comment";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    assert(tokens.empty());
    std::cout << "Test passed!" << std::endl;
}

void test_multi_line_comment() {
    std::cout << "Running test: test_multi_line_comment..." << std::endl;
    std::string source = "/* this is a multi-line comment */";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    assert(tokens.empty());
    std::cout << "Test passed!" << std::endl;
}

int main() {
    test_single_line_comment();
    test_multi_line_comment();
    return 0;
}
