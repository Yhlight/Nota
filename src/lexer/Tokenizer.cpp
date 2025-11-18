#include "Tokenizer.h"
#include <unordered_map>
#include <cctype>

static const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"let", TokenType::LET},
    {"mut", TokenType::MUT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"end", TokenType::END},
    {"match", TokenType::MATCH},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"do", TokenType::DO},
    {"func", TokenType::FUNC},
    {"class", TokenType::CLASS},
    {"return", TokenType::RETURN},
    {"import", TokenType::IMPORT},
    {"package", TokenType::PACKAGE},
    {"as", TokenType::AS},
    {"none", TokenType::NONE},
    {"true", TokenType::BOOL},
    {"false", TokenType::BOOL}
};

Tokenizer::Tokenizer(const std::string& source) : source_(source) {}

std::vector<Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = next_token();
        tokens.push_back(token);
    } while (token.type != TokenType::END_OF_FILE);
    return tokens;
}

Token Tokenizer::next_token() {
    skip_whitespace_and_comments();

    if (is_at_end()) {
        return make_token(TokenType::END_OF_FILE, "");
    }

    char c = peek();

    if (isalpha(c) || c == '_') {
        return identifier();
    }

    if (isdigit(c)) {
        return number();
    }

    if (c == '"') {
        return string();
    }

    return punctuator();
}

Token Tokenizer::identifier() {
    std::string lexeme;
    lexeme += advance();
    while (isalnum(peek()) || peek() == '_') {
        lexeme += advance();
    }
    if (KEYWORDS.count(lexeme)) {
        return make_token(KEYWORDS.at(lexeme), lexeme);
    }
    return make_token(TokenType::IDENTIFIER, lexeme);
}

Token Tokenizer::number() {
    std::string lexeme;
    lexeme += advance();
    while (isdigit(peek())) {
        lexeme += advance();
    }
    if (peek() == '.') {
        lexeme += advance();
        while (isdigit(peek())) {
            lexeme += advance();
        }
        return make_token(TokenType::FLOAT, lexeme);
    }
    return make_token(TokenType::INTEGER, lexeme);
}

Token Tokenizer::string() {
    advance(); // consume the opening quote
    std::string lexeme;
    while (peek() != '"' && !is_at_end()) {
        lexeme += advance();
    }
    if (is_at_end()) {
        return make_token(TokenType::UNRECOGNIZED, lexeme);
    }
    advance(); // consume the closing quote
    return make_token(TokenType::STRING, lexeme);
}

Token Tokenizer::punctuator() {
    char c = advance();
    switch (c) {
        case '+':
            if (peek() == '=') { advance(); return make_token(TokenType::PLUS_ASSIGN, "+="); }
            if (peek() == '+') { advance(); return make_token(TokenType::INC, "++"); }
            return make_token(TokenType::PLUS, "+");
        case '-':
            if (peek() == '=') { advance(); return make_token(TokenType::MINUS_ASSIGN, "-="); }
            if (peek() == '-') { advance(); return make_token(TokenType::DEC, "--"); }
            return make_token(TokenType::MINUS, "-");
        case '*':
            if (peek() == '=') { advance(); return make_token(TokenType::STAR_ASSIGN, "*="); }
            return make_token(TokenType::STAR, "*");
        case '/':
            if (peek() == '=') { advance(); return make_token(TokenType::SLASH_ASSIGN, "/="); }
            return make_token(TokenType::SLASH, "/");
        case '%':
            if (peek() == '=') { advance(); return make_token(TokenType::PERCENT_ASSIGN, "%="); }
            return make_token(TokenType::PERCENT, "%");
        case '=':
            if (peek() == '=') { advance(); return make_token(TokenType::EQ, "=="); }
            if (peek() == '>') { advance(); return make_token(TokenType::ARROW, "=>"); }
            return make_token(TokenType::ASSIGN, "=");
        case '!':
            if (peek() == '=') { advance(); return make_token(TokenType::NEQ, "!="); }
            return make_token(TokenType::NOT, "!");
        case '>':
            if (peek() == '=') { advance(); return make_token(TokenType::GTE, ">="); }
            if (peek() == '>') { advance(); return make_token(TokenType::RSHIFT, ">>"); }
            return make_token(TokenType::GT, ">");
        case '<':
            if (peek() == '=') { advance(); return make_token(TokenType::LTE, "<="); }
            if (peek() == '<') { advance(); return make_token(TokenType::LSHIFT, "<<"); }
            return make_token(TokenType::LT, "<");
        case '&':
            if (peek() == '&') { advance(); return make_token(TokenType::AND, "&&"); }
            return make_token(TokenType::AMP, "&");
        case '|':
            if (peek() == '|') { advance(); return make_token(TokenType::OR, "||"); }
            return make_token(TokenType::PIPE, "|");
        case '^': return make_token(TokenType::CARET, "^");
        case '~': return make_token(TokenType::TILDE, "~");
        case '(': return make_token(TokenType::LPAREN, "(");
        case ')': return make_token(TokenType::RPAREN, ")");
        case '{': return make_token(TokenType::LBRACE, "{");
        case '}': return make_token(TokenType::RBRACE, "}");
        case '[': return make_token(TokenType::LBRACKET, "[");
        case ']': return make_token(TokenType::RBRACKET, "]");
        case ',': return make_token(TokenType::COMMA, ",");
        case '.': return make_token(TokenType::DOT, ".");
        case ':': return make_token(TokenType::COLON, ":");
        case ';': return make_token(TokenType::SEMICOLON, ";");
        case '\n': return make_token(TokenType::NEWLINE, "\n");
    }

    return make_token(TokenType::UNRECOGNIZED, std::string(1, c));
}


Token Tokenizer::make_token(TokenType type, const std::string& lexeme) {
    return Token{type, lexeme, location_};
}

char Tokenizer::peek() {
    if (is_at_end()) return '\0';
    return source_[current_];
}

char Tokenizer::advance() {
    if (!is_at_end()) {
        char c = source_[current_++];
        if (c == '\n') {
            location_.line++;
            location_.column = 1;
        } else {
            location_.column++;
        }
        return c;
    }
    return '\0';
}

bool Tokenizer::is_at_end() {
    return current_ >= source_.size();
}

void Tokenizer::skip_whitespace_and_comments() {
    while (!is_at_end()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else if (c == '/') {
            if (current_ + 1 < source_.size() && source_[current_ + 1] == '/') {
                while (peek() != '\n' && !is_at_end()) {
                    advance();
                }
            } else if (current_ + 1 < source_.size() && source_[current_ + 1] == '*') {
                advance(); // consume /
                advance(); // consume *
                while (!is_at_end()) {
                    if (peek() == '*' && current_ + 1 < source_.size() && source_[current_ + 1] == '/') {
                        break;
                    }
                    advance();
                }
                if (!is_at_end()) {
                    advance(); // consume *
                    advance(); // consume /
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
}
