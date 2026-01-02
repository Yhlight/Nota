#include "Lexer.hpp"
#include <unordered_map>
#include <cctype>

namespace Nota {

static const std::unordered_map<std::string, TokenType> keywords = {
    {"App",    TokenType::KW_APP},
    {"Row",    TokenType::KW_ROW},
    {"Col",    TokenType::KW_COL},
    {"Rect",   TokenType::KW_RECT},
    {"Text",   TokenType::KW_TEXT},
    {"Item",   TokenType::KW_ITEM},
    {"this",   TokenType::KW_THIS},
    {"parent", TokenType::KW_PARENT},
};

Lexer::Lexer(const std::string& source) : source_(source) {}

std::vector<Token> Lexer::ScanTokens() {
    while (!IsAtEnd()) {
        start_ = current_;
        ScanToken();
    }

    tokens_.push_back({TokenType::END_OF_FILE, "", line_, column_});
    return tokens_;
}

void Lexer::ScanToken() {
    char c = Advance();
    switch (c) {
        // Single-character tokens
        case '{': tokens_.push_back(MakeToken(TokenType::LEFT_BRACE)); break;
        case '}': tokens_.push_back(MakeToken(TokenType::RIGHT_BRACE)); break;
        case '[': tokens_.push_back(MakeToken(TokenType::LEFT_BRACKET)); break;
        case ']': tokens_.push_back(MakeToken(TokenType::RIGHT_BRACKET)); break;
        case ':': tokens_.push_back(MakeToken(TokenType::COLON)); break;
        case '.': tokens_.push_back(MakeToken(TokenType::DOT)); break;
        case '=': tokens_.push_back(MakeToken(TokenType::EQUAL)); break;
        case ';': tokens_.push_back(MakeToken(TokenType::SEMICOLON)); break;

        // Comments and division
        case '/':
            if (Match('/')) {
                // A single-line comment goes until the end of the line.
                while (Peek() != '\n' && !IsAtEnd()) Advance();
            } else if (Match('*')) {
                // A multi-line comment
                while (!(Peek() == '*' && PeekNext() == '/') && !IsAtEnd()) {
                    if (Peek() == '\n') {
                        line_++;
                        column_ = 0;
                    }
                    Advance();
                }
                if (!IsAtEnd()) {
                    Advance(); // Consume '*'
                    Advance(); // Consume '/'
                }
            } else {
                tokens_.push_back(MakeToken(TokenType::SLASH));
            }
            break;

        // Whitespace
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;

        case '\n':
            line_++;
            column_ = 0;
            break;

        // Literals
        case '"': HandleString(); break;
        case '#':
            // Color hex code
            while (isxdigit(Peek())) Advance();
            tokens_.push_back(MakeToken(TokenType::COLOR_HEX));
            break;

        default:
            if (isdigit(c)) {
                HandleNumber();
            } else if (isalpha(c) || c == '_') {
                HandleIdentifier();
            } else {
                tokens_.push_back(MakeToken(TokenType::UNKNOWN));
            }
            break;
    }
}

char Lexer::Advance() {
    column_++;
    return source_[current_++];
}

bool Lexer::IsAtEnd() {
    return current_ >= source_.length();
}

bool Lexer::Match(char expected) {
    if (IsAtEnd()) return false;
    if (source_[current_] != expected) return false;
    current_++;
    column_++;
    return true;
}

char Lexer::Peek() {
    if (IsAtEnd()) return '\0';
    return source_[current_];
}

char Lexer::PeekNext() {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

void Lexer::HandleString() {
    while (Peek() != '"' && !IsAtEnd()) {
        if (Peek() == '\n') {
            line_++;
            column_ = 0;
        }
        Advance();
    }

    if (IsAtEnd()) {
        // Unterminated string.
        tokens_.push_back(MakeToken(TokenType::UNKNOWN));
        return;
    }

    // The closing ".
    Advance();

    // Trim the surrounding quotes.
    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    tokens_.push_back({TokenType::STRING, value, line_, column_ - (int)value.length()});
}

void Lexer::HandleNumber() {
    while (isdigit(Peek())) Advance();

    // Look for a fractional part.
    if (Peek() == '.' && isdigit(PeekNext())) {
        // Consume the "."
        Advance();
        while (isdigit(Peek())) Advance();
    }

    if(Peek() == '%')
    {
        Advance();
        tokens_.push_back(MakeToken(TokenType::PERCENTAGE));
    }
    else
    {
        tokens_.push_back(MakeToken(TokenType::NUMBER));
    }
}

void Lexer::HandleIdentifier() {
    while (isalnum(Peek()) || Peek() == '_') Advance();

    std::string text = source_.substr(start_, current_ - start_);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        tokens_.push_back(MakeToken(it->second));
    } else {
        tokens_.push_back(MakeToken(TokenType::IDENTIFIER));
    }
}

Token Lexer::MakeToken(TokenType type) const {
    return {type, source_.substr(start_, current_ - start_), line_, column_ - (current_ - start_)};
}

} // namespace Nota
