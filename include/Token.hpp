#ifndef NOTA_TOKEN_HPP
#define NOTA_TOKEN_HPP

#include <string>
#include <string_view>
#include <sstream>

namespace Nota {

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN,   // (
    RIGHT_PAREN,  // )
    LEFT_BRACE,   // {
    RIGHT_BRACE,  // }
    LEFT_BRACKET, // [
    RIGHT_BRACKET,// ]
    COLON,        // :
    DOT,          // .
    EQUAL,        // =
    SLASH,        // /
    SEMICOLON,    // ;

    // Literals.
    IDENTIFIER,
    STRING,       // "hello"
    NUMBER,       // 123 or 12.3
    PERCENTAGE,   // 100%
    COLOR_HEX,    // #ff00ff

    // Keywords for component types
    KW_APP,
    KW_ROW,
    KW_COL,
    KW_RECT,
    KW_TEXT,
    KW_ITEM,

    // Other keywords
    KW_THIS,
    KW_PARENT,

    // Special tokens
    END_OF_FILE,
    UNKNOWN
};

// Helper function to convert TokenType to string for debugging
inline std::string TokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_PAREN:   return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN:  return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE:   return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE:  return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET:return "RIGHT_BRACKET";
        case TokenType::COLON:        return "COLON";
        case TokenType::DOT:          return "DOT";
        case TokenType::EQUAL:        return "EQUAL";
        case TokenType::SLASH:        return "SLASH";
        case TokenType::SEMICOLON:    return "SEMICOLON";
        case TokenType::IDENTIFIER:   return "IDENTIFIER";
        case TokenType::STRING:       return "STRING";
        case TokenType::NUMBER:       return "NUMBER";
        case TokenType::PERCENTAGE:   return "PERCENTAGE";
        case TokenType::COLOR_HEX:    return "COLOR_HEX";
        case TokenType::KW_APP:       return "KW_APP";
        case TokenType::KW_ROW:       return "KW_ROW";
        case TokenType::KW_COL:       return "KW_COL";
        case TokenType::KW_RECT:      return "KW_RECT";
        case TokenType::KW_TEXT:      return "KW_TEXT";
        case TokenType::KW_ITEM:      return "KW_ITEM";
        case TokenType::KW_THIS:      return "KW_THIS";
        case TokenType::KW_PARENT:    return "KW_PARENT";
        case TokenType::END_OF_FILE:  return "END_OF_FILE";
        case TokenType::UNKNOWN:      return "UNKNOWN";
        default:                      return "UNHANDLED_TOKEN_TYPE";
    }
}


struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    std::string ToString() const {
        std::stringstream ss;
        ss << "Token(Type: " << TokenTypeToString(type)
           << ", Lexeme: '" << lexeme
           << "', Line: " << line
           << ", Col: " << column << ")";
        return ss.str();
    }
};

} // namespace Nota

#endif // NOTA_TOKEN_HPP
