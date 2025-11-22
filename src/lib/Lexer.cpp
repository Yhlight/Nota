#include "Lexer.h"

#include <map>

namespace nota {

namespace {
    const std::map<std::string, TokenType> keywords = {
        {"let",    TokenType::LET},
        {"mut",    TokenType::MUT},
        {"fn",     TokenType::FN},
        {"class",  TokenType::CLASS},
        {"this",   TokenType::THIS},
        {"if",     TokenType::IF},
        {"else",   TokenType::ELSE},
        {"while",  TokenType::WHILE},
        {"for",    TokenType::FOR},
        {"do",     TokenType::DO},
        {"return", TokenType::RETURN},
        {"true",   TokenType::TRUE},
        {"false",  TokenType::FALSE},
        {"end",    TokenType::END},
        {"import", TokenType::IMPORT},
        {"as",     TokenType::AS},
        {"package",TokenType::PACKAGE},
        {"none",   TokenType::NONE}
    };
}


Lexer::Lexer(const std::string& source) : source_(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start_ = current_;
        scanToken();
    }

    tokens_.push_back({TokenType::END_OF_FILE, "", {}, line_});
    return tokens_;
}

bool Lexer::isAtEnd() const {
    return current_ >= source_.length();
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LPAREN); break;
        case ')': addToken(TokenType::RPAREN); break;
        case '{': addToken(TokenType::LBRACE); break;
        case '}': addToken(TokenType::RBRACE); break;
        case '[': addToken(TokenType::LBRACKET); break;
        case ']': addToken(TokenType::RBRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case ':':
            addToken(match(':') ? TokenType::COLON_COLON : TokenType::COLON);
            break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '+':
            if (match('+')) {
                addToken(TokenType::PLUS_PLUS);
            } else if (match('=')) {
                addToken(TokenType::PLUS_ASSIGN);
            } else {
                addToken(TokenType::PLUS);
            }
            break;
        case '-':
            addToken(match('=') ? TokenType::MINUS_ASSIGN : TokenType::MINUS);
            break;
        case '*':
            addToken(match('=') ? TokenType::STAR_ASSIGN : TokenType::STAR);
            break;
        case '/':
            addToken(match('=') ? TokenType::SLASH_ASSIGN : TokenType::SLASH);
            break;
        case '%':
            addToken(match('=') ? TokenType::PERCENT_ASSIGN : TokenType::PERCENT);
            break;

        case '=':
            addToken(match('>') ? TokenType::ARROW : (match('=') ? TokenType::EQUALS : TokenType::ASSIGN));
            break;
        case '!':
            addToken(match('=') ? TokenType::NOT_EQUALS : TokenType::NOT);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUALS : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUALS : TokenType::GREATER);
            break;

        case '&':
            if (match('&')) {
                addToken(TokenType::AND);
            }
            break;

        case '|':
            if (match('|')) {
                addToken(TokenType::OR);
            }
            break;

        case '"': string(); break;

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace
            break;
        case '\n':
            addToken(TokenType::NEWLINE);
            line_++;
            break;

        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                report(line_, "", "Unexpected character.");
            }
            break;
    }
}

char Lexer::advance() {
    return source_[current_++];
}

void Lexer::addToken(TokenType type) {
    addToken(type, {});
}

void Lexer::addToken(TokenType type, const std::variant<std::monostate, int, double, std::string, bool>& literal) {
    std::string text = source_.substr(start_, current_ - start_);
    tokens_.push_back({type, text, literal, line_});
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;

    current_++;
    return true;
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

void Lexer::number() {
    while (isDigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the "."
        advance();

        while (isDigit(peek())) advance();
        addToken(TokenType::FLOAT, std::stod(source_.substr(start_, current_ - start_)));
        return;
    }

    addToken(TokenType::INTEGER, std::stoi(source_.substr(start_, current_ - start_)));
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string text = source_.substr(start_, current_ - start_);
    TokenType type;
    auto it = keywords.find(text);
    if (it == keywords.end()) {
        type = TokenType::IDENTIFIER;
    } else {
        type = it->second;
    }

    addToken(type);
}


bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

char Lexer::peekNext() const {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line_++;
        advance();
    }

    if (isAtEnd()) {
        report(line_, "", "Unterminated string.");
        return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    addToken(TokenType::STRING, value);
}

void Lexer::report(int line, const std::string& where, const std::string& message) {
    hadError_ = true;
    // In a real implementation, this would likely print to stderr
    // For now, we just set the flag.
}

} // namespace nota
