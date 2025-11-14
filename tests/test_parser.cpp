#include "../src/parser.h"
#include <iostream>
#include <cassert>

void testParseLetStatement() {
    std::string input = "let x = 5\nmut y = 10.5\nlet z = \"hello\"";
    Lexer l(input);
    Parser p(l);

    auto program = p.parseProgram();

    assert(program != nullptr);
    assert(program->statements.size() == 3);

    auto letStmt1 = dynamic_cast<LetStatement*>(program->statements[0].get());
    assert(letStmt1 != nullptr);
    assert(letStmt1->token.type == TokenType::LET);
    assert(letStmt1->name.literal == "x");
    assert(letStmt1->value->String() == "5");

    auto letStmt2 = dynamic_cast<LetStatement*>(program->statements[1].get());
    assert(letStmt2 != nullptr);
    assert(letStmt2->token.type == TokenType::MUT);
    assert(letStmt2->name.literal == "y");
    assert(letStmt2->value->String() == "10.5");

    auto letStmt3 = dynamic_cast<LetStatement*>(program->statements[2].get());
    assert(letStmt3 != nullptr);
    assert(letStmt3->token.type == TokenType::LET);
    assert(letStmt3->name.literal == "z");
    assert(letStmt3->value->String() == "hello");


    std::cout << "testParseLetStatement passed" << std::endl;
}

int main() {
    testParseLetStatement();
    return 0;
}
