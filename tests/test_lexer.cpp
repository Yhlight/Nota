#include "../src/lexer.h"
#include <iostream>
#include <vector>
#include <cassert>

void testNextToken() {
    std::string input = "let x = 5\nmut y = 10.5\nlet z = \"hello\"";
    Lexer l(input);

    std::vector<Token> expected_tokens = {
        {TokenType::LET, "let"},
        {TokenType::IDENTIFIER, "x"},
        {TokenType::ASSIGN, "="},
        {TokenType::INTEGER, "5"},
        {TokenType::MUT, "mut"},
        {TokenType::IDENTIFIER, "y"},
        {TokenType::ASSIGN, "="},
        {TokenType::FLOAT, "10.5"},
        {TokenType::LET, "let"},
        {TokenType::IDENTIFIER, "z"},
        {TokenType::ASSIGN, "="},
        {TokenType::STRING, "hello"},
        {TokenType::END_OF_FILE, ""},
    };

    for (const auto& expected_token : expected_tokens) {
        Token actual_token = l.nextToken();
        assert(actual_token.type == expected_token.type);
        assert(actual_token.literal == expected_token.literal);
    }
    std::cout << "testNextToken passed" << std::endl;
}

int main() {
    testNextToken();
    return 0;
}
