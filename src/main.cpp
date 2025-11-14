#include <iostream>
#include "parser.h"

int main() {
    std::string input = "let x = 5\nmut y = 10.5\nlet z = \"hello\"";
    Lexer l(input);
    Parser p(l);

    auto program = p.parseProgram();

    std::cout << program->String() << std::endl;

    return 0;
}
