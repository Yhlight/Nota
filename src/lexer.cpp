#include "lexer.h"
#include <cctype>
#include <unordered_map>
#include <stdexcept>

Lexer::Lexer(const std::string& source)
    : source(source), pos(0), len(source.length()), line(1), column(1) {}

char Lexer::peek(int offset) const {
    if (pos + offset >= len) return '\0';
    return source[pos + offset];
}

char Lexer::advance() {
    if (pos >= len) return '\0';
    char current = source[pos++];
    if (current == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return current;
}

bool Lexer::match(char expected) {
    if (peek() == expected) {
        advance();
        return true;
    }
    return false;
}

Token Lexer::makeToken(TokenType type, std::string value) {
    // Correct column logic: the token started 'value.length()' characters ago.
    // However, since we've already advanced, 'column' points to the *next* char.
    // So the start column is `column - value.length()`.
    // But dealing with newlines makes this tricky. For simplicity, we store the start position
    // if we tracked it, but here we can just approximation or store start_col in tokenize loop.
    return Token{type, value, line, column - (int)value.length()};
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        if (std::isspace(c)) {
            advance();
        } else if (c == '/') {
            if (peek(1) == '/') {
                // Single line comment
                while (peek() != '\n' && peek() != '\0') advance();
            } else if (peek(1) == '*') {
                // Multi line comment
                advance(); advance(); // Consume /*
                while (!(peek() == '*' && peek(1) == '/') && peek() != '\0') {
                    advance();
                }
                if (peek() == '*' && peek(1) == '/') {
                    advance(); advance(); // Consume */
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

Token Lexer::readIdentifierOrKeyword() {
    std::string text;
    // Identifiers can contain alphanumeric and underscores, but start with alpha or underscore
    // Note: Nota docs example shows "Box", "width".
    while (std::isalnum(peek()) || peek() == '_') {
        text += advance();
    }

    static const std::unordered_map<std::string, TokenType> keywords = {
        {"Item", TokenType::KEYWORD_ITEM},
        {"App", TokenType::KEYWORD_APP},
        {"Row", TokenType::KEYWORD_ROW},
        {"Col", TokenType::KEYWORD_COL},
        {"Rect", TokenType::KEYWORD_RECT},
        {"Text", TokenType::KEYWORD_TEXT},
        {"property", TokenType::KEYWORD_PROPERTY},
        {"import", TokenType::KEYWORD_IMPORT},
        {"export", TokenType::KEYWORD_EXPORT},
        {"Struct", TokenType::KEYWORD_STRUCT},
        {"State", TokenType::KEYWORD_STATE},
        {"if", TokenType::KEYWORD_IF},
        {"else", TokenType::KEYWORD_ELSE},
        {"delegate", TokenType::KEYWORD_DELEGATE},
        {"for", TokenType::KEYWORD_FOR},
        {"package", TokenType::KEYWORD_PACKAGE},
    };

    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return makeToken(it->second, text);
    }
    return makeToken(TokenType::IDENTIFIER, text);
}

Token Lexer::readNumber() {
    std::string text;
    while (std::isdigit(peek())) {
        text += advance();
    }
    if (peek() == '.' && std::isdigit(peek(1))) {
        text += advance();
        while (std::isdigit(peek())) {
            text += advance();
        }
    }
    // Handle % for percentages (e.g. 100%)
    if (peek() == '%') {
        // We could treat this as a separate token or part of the number literal.
        // For now, let's treat it as part of the number string or separate?
        // Nota.md says "width: 100%".
        // Let's include it in the value string for now, it's easier for parsing "Value".
        text += advance();
    }
    return makeToken(TokenType::NUMBER_LITERAL, text);
}

Token Lexer::readString() {
    // Assume string starts with "
    advance(); // consume "
    std::string text;
    while (peek() != '"' && peek() != '\0') {
        if (peek() == '\\') {
            advance(); // skip backslash
            // basic escape handling
            if (peek() == '"') text += '"';
            else if (peek() == 'n') text += '\n';
            else text += peek();
            advance();
        } else {
            text += advance();
        }
    }
    if (peek() == '"') advance(); // consume closing "
    return makeToken(TokenType::STRING_LITERAL, text);
}

Token Lexer::readHexColor() {
    // Starts with #
    std::string text;
    text += advance(); // consume #
    while (std::isxdigit(peek())) {
        text += advance();
    }
    return makeToken(TokenType::HEX_COLOR_LITERAL, text);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        skipWhitespace();
        if (peek() == '\0') break;

        char c = peek();
        int start_col = column; // Keep track for accurate reporting if needed

        if (std::isalpha(c) || c == '_') {
            tokens.push_back(readIdentifierOrKeyword());
        } else if (std::isdigit(c)) {
            tokens.push_back(readNumber());
        } else if (c == '"') {
            tokens.push_back(readString());
        } else if (c == '#') {
            tokens.push_back(readHexColor());
        } else {
            // Single char tokens
            advance();
            switch (c) {
                case '{': tokens.push_back(makeToken(TokenType::LBRACE, "{")); break;
                case '}': tokens.push_back(makeToken(TokenType::RBRACE, "}")); break;
                case '(': tokens.push_back(makeToken(TokenType::LPAREN, "(")); break;
                case ')': tokens.push_back(makeToken(TokenType::RPAREN, ")")); break;
                case '[': tokens.push_back(makeToken(TokenType::LBRACKET, "[")); break;
                case ']': tokens.push_back(makeToken(TokenType::RBRACKET, "]")); break;
                case ':': tokens.push_back(makeToken(TokenType::COLON, ":")); break;
                case ';': tokens.push_back(makeToken(TokenType::SEMICOLON, ";")); break;
                case '.': tokens.push_back(makeToken(TokenType::DOT, ".")); break;
                case ',': tokens.push_back(makeToken(TokenType::COMMA, ",")); break;
                case '+': tokens.push_back(makeToken(TokenType::PLUS, "+")); break;
                case '-': tokens.push_back(makeToken(TokenType::MINUS, "-")); break;
                case '*': tokens.push_back(makeToken(TokenType::STAR, "*")); break;
                case '/': tokens.push_back(makeToken(TokenType::SLASH, "/")); break;
                case '=': tokens.push_back(makeToken(TokenType::EQUAL, "=")); break;
                case '<': tokens.push_back(makeToken(TokenType::LESS, "<")); break;
                case '>': tokens.push_back(makeToken(TokenType::GREATER, ">")); break;
                default:
                    // Unknown char
                    // For now, ignore or throw?
                    // Let's print error to stderr and continue? Or throw exception?
                    // throw std::runtime_error(std::string("Unexpected character: ") + c);
                    break;
            }
        }
    }
    tokens.push_back(Token{TokenType::EOF_TOKEN, "", line, column});
    return tokens;
}
