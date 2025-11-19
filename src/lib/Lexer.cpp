#include "Lexer.h"

#include <map>

// A map to look up keywords
static std::map<std::string, TokenType> keywords;

static void initializeKeywords() {
    keywords["let"] = TokenType::LET;
    keywords["mut"] = TokenType::MUT;
    keywords["func"] = TokenType::FUNC;
    keywords["class"] = TokenType::CLASS;
    keywords["if"] = TokenType::IF;
    keywords["else"] = TokenType::ELSE;
    keywords["end"] = TokenType::END;
    keywords["match"] = TokenType::MATCH;
    keywords["while"] = TokenType::WHILE;
    keywords["for"] = TokenType::FOR;
    keywords["do"] = TokenType::DO;
    keywords["return"] = TokenType::RETURN;
    keywords["true"] = TokenType::TRUE;
    keywords["false"] = TokenType::FALSE;
    keywords["none"] = TokenType::NONE;
    keywords["import"] = TokenType::IMPORT;
    keywords["as"] = TokenType::AS;
    keywords["package"] = TokenType::PACKAGE;
}

Lexer::Lexer(std::string source) : source(std::move(source)) {
    initializeKeywords();
}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    std::vector<Token> filteredTokens;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].type == TokenType::NEWLINE) {
            if (i > 0) {
                TokenType prevType = tokens[i - 1].type;
                if (prevType == TokenType::PLUS || prevType == TokenType::MINUS ||
                    prevType == TokenType::STAR || prevType == TokenType::SLASH ||
                    prevType == TokenType::EQUAL || prevType == TokenType::LEFT_PAREN ||
                    prevType == TokenType::LEFT_BRACKET) {
                    continue;
                }
            }
        }
        filteredTokens.push_back(tokens[i]);
    }

    filteredTokens.push_back({TokenType::END_OF_FILE, "", std::monostate{}, line});
    return filteredTokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    return source[current++];
}

void Lexer::addToken(TokenType type) {
    addToken(type, std::monostate{});
}

void Lexer::addToken(TokenType type, const std::variant<std::monostate, int, double, std::string, bool>& literal) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, literal, line});
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case '-': addToken(match('-') ? TokenType::MINUS_MINUS : (match('=') ? TokenType::MINUS_EQUAL : TokenType::MINUS)); break;
        case '+': addToken(match('+') ? TokenType::PLUS_PLUS : (match('=') ? TokenType::PLUS_EQUAL : TokenType::PLUS)); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '*': addToken(match('=') ? TokenType::STAR_EQUAL : TokenType::STAR); break;
        case '%': addToken(TokenType::PERCENT); break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                // A multi-line comment
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') line++;
                    advance();
                }
                if (isAtEnd()) {
                    error("Unterminated multi-line comment.");
                } else {
                    advance(); // consume the '*'
                    advance(); // consume the '/'
                }
            }
            else {
                addToken(match('=') ? TokenType::SLASH_EQUAL : TokenType::SLASH);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;
        case '\n':
            line++;
            addToken(TokenType::NEWLINE);
            break;
        case '"': string(); break;
        default:
            if (isdigit(c)) {
                number();
            } else if (isalpha(c) || c == '_') {
                identifier();
            }
            else {
                error("Unexpected character.");
            }
            break;
    }
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;
    if (keywords.count(text)) {
        type = keywords[text];
    }
    addToken(type);
}

void Lexer::number() {
    bool isFloat = false;
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        isFloat = true;
        // Consume the "."
        advance();

        while (isdigit(peek())) advance();
    }

    std::string text = source.substr(start, current - start);
    if(isFloat) {
        addToken(TokenType::NUMBER_FLOAT, std::stod(text));
    } else {
        addToken(TokenType::NUMBER_INT, std::stoi(text));
    }
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        error("Unterminated string.");
        return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;

    current++;
    return true;
}

void Lexer::error(const std::string& message) {
    errors.push_back({line, message});
}

bool Lexer::hasError() const {
    return !errors.empty();
}

const std::vector<NotaError>& Lexer::getErrors() const {
    return errors;
}
