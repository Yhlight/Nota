#include "include/Parser.hpp"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<ComponentNode> Parser::parse() {
    return parseComponent();
}

// Private methods
std::unique_ptr<ComponentNode> Parser::parseComponent() {
    Token type = advance(); // Consume component type
    auto component = std::make_unique<ComponentNode>(type);

    consume(TokenType::LEFT_BRACE, "Expect '{' after component type.");

    while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (tokens[current + 1].type == TokenType::COLON) {
            component->properties.push_back(parseProperty());
        } else {
            component->children.push_back(parseComponent());
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after component body.");
    return component;
}

std::unique_ptr<PropertyNode> Parser::parseProperty() {
    Token key = consume(TokenType::IDENTIFIER, "Expect property name.");
    consume(TokenType::COLON, "Expect ':' after property name.");
    Token value = advance(); // Consume value
    return std::make_unique<PropertyNode>(key, value);
}

bool Parser::match(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (peek().type == type) return advance();
    throw std::runtime_error(message);
}


bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

Token Parser::peek() {
    return tokens[current];
}
