#include <iostream>
#include "Lexer.h"
#include "Token.h"
#include <vector>

// A helper function to convert TokenType to string for printing
std::string tokenTypeToString(nota::TokenType type) {
    switch (type) {
        case nota::TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case nota::TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case nota::TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case nota::TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case nota::TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case nota::TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case nota::TokenType::COMMA: return "COMMA";
        case nota::TokenType::DOT: return "DOT";
        case nota::TokenType::MINUS: return "MINUS";
        case nota::TokenType::PLUS: return "PLUS";
        case nota::TokenType::SLASH: return "SLASH";
        case nota::TokenType::STAR: return "STAR";
        case nota::TokenType::MODULO: return "MODULO";
        case nota::TokenType::COLON: return "COLON";
        case nota::TokenType::BANG: return "BANG";
        case nota::TokenType::BANG_EQUAL: return "BANG_EQUAL";
        case nota::TokenType::EQUAL: return "EQUAL";
        case nota::TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case nota::TokenType::GREATER: return "GREATER";
        case nota::TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case nota::TokenType::LESS: return "LESS";
        case nota::TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case nota::TokenType::PLUS_EQUAL: return "PLUS_EQUAL";
        case nota::TokenType::MINUS_EQUAL: return "MINUS_EQUAL";
        case nota::TokenType::STAR_EQUAL: return "STAR_EQUAL";
        case nota::TokenType::SLASH_EQUAL: return "SLASH_EQUAL";
        case nota::TokenType::MODULO_EQUAL: return "MODULO_EQUAL";
        case nota::TokenType::PLUS_PLUS: return "PLUS_PLUS";
        case nota::TokenType::MINUS_MINUS: return "MINUS_MINUS";
        case nota::TokenType::IDENTIFIER: return "IDENTIFIER";
        case nota::TokenType::STRING: return "STRING";
        case nota::TokenType::INTEGER: return "INTEGER";
        case nota::TokenType::FLOAT: return "FLOAT";
        case nota::TokenType::LET: return "LET";
        case nota::TokenType::MUT: return "MUT";
        case nota::TokenType::IF: return "IF";
        case nota::TokenType::ELSE: return "ELSE";
        case nota::TokenType::END: return "END";
        case nota::TokenType::MATCH: return "MATCH";
        case nota::TokenType::WHILE: return "WHILE";
        case nota::TokenType::FOR: return "FOR";
        case nota::TokenType::DO: return "DO";
        case nota::TokenType::FN: return "FN";
        case nota::TokenType::RETURN: return "RETURN";
        case nota::TokenType::CLASS: return "CLASS";
        case nota::TokenType::NEW: return "NEW";
        case nota::TokenType::THIS: return "THIS";
        case nota::TokenType::IMPORT: return "IMPORT";
        case nota::TokenType::AS: return "AS";
        case nota::TokenType::PACKAGE: return "PACKAGE";
        case nota::TokenType::NONE: return "NONE";
        case nota::TokenType::TRUE: return "TRUE";
        case nota::TokenType::FALSE: return "FALSE";
        case nota::TokenType::NEWLINE: return "NEWLINE";
        case nota::TokenType::END_OF_FILE: return "END_OF_FILE";
        case nota::TokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
        case nota::TokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
        case nota::TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

int main() {
    std::string source = "let a = 10 + 20";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();

    for (const auto& token : tokens) {
        std::cout << "Type: " << tokenTypeToString(token.type)
                  << ", Lexeme: '" << token.lexeme
                  << "', Line: " << token.line
                  << ", Column: " << token.column << std::endl;
    }

    return 0;
}
