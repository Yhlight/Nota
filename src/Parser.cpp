#include "Parser.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<Component> Parser::parse() {
    return component();
}

std::unique_ptr<Component> Parser::component() {
    Token name = advance(); // Consume component name

    consume(TokenType::LEFT_BRACE, "Expect '{' after component name.");

    std::vector<std::unique_ptr<Property>> properties;
    std::vector<std::unique_ptr<Component>> children;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (peek().type == TokenType::IDENTIFIER && tokens[current + 1].type == TokenType::COLON) {
            properties.push_back(property());
        } else {
            children.push_back(component());
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after component block.");

    return std::make_unique<Component>(name, std::move(properties), std::move(children));
}

std::unique_ptr<Property> Parser::property() {
    Token name = consume(TokenType::IDENTIFIER, "Expect property name.");
    consume(TokenType::COLON, "Expect ':' after property name.");
    std::unique_ptr<Expr> value = expression();

    // Optional semicolon
    if (match({TokenType::SEMICOLON})) {
        // consumed
    }

    return std::make_unique<Property>(name, std::move(value));
}

std::unique_ptr<Expr> Parser::expression() {
    if (match({TokenType::NUMBER, TokenType::STRING, TokenType::IDENTIFIER})) {
        return std::make_unique<Literal>(previous());
    }

    return nullptr; // Should not happen in a valid program
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}
