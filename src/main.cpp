#include <iostream>
#include <vector>
#include <string>
#include "include/Lexer.hpp"
#include "include/Token.hpp"

// Helper to convert TokenType to string for printing
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::ITEM: return "ITEM";
        case TokenType::APP: return "APP";
        case TokenType::ROW: return "ROW";
        case TokenType::COL: return "COL";
        case TokenType::RECT: return "RECT";
        case TokenType::TEXT: return "TEXT";
        case TokenType::BUTTON: return "BUTTON";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::AS: return "AS";
        case TokenType::PACKAGE: return "PACKAGE";
        case TokenType::EXPORT: return "EXPORT";
        case TokenType::PROPERTY: return "PROPERTY";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::STATES: return "STATES";
        case TokenType::STATE: return "STATE";
        case TokenType::WHEN: return "WHEN";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::FOR: return "FOR";
        case TokenType::STRUCT: return "STRUCT";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

int main(int argc, char* argv[]) {
    std::string source = R"(
        // Nota Lexer Demonstration
        App {
            width: 100
            /* A simple rectangle */
            Rect {
                color: "blue"
            }
        }
    )";

    std::cout << "--- Tokenizing Nota Source ---" << std::endl;
    std::cout << source << std::endl;
    std::cout << "-----------------------------" << std::endl;

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    for (const auto& token : tokens) {
        std::cout << "Type: " << tokenTypeToString(token.type)
                  << ", Lexeme: '" << token.lexeme
                  << "', Line: " << token.line << std::endl;
    }

    return 0;
}
