#include "Parser.hpp"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();
    while (!isAtEnd()) {
        auto stmt = statement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
    }
    return program;
}

std::unique_ptr<Statement> Parser::statement() {
    if (peek().type == TokenType::IDENTIFIER ||
        peek().type == TokenType::APP ||
        peek().type == TokenType::ROW ||
        peek().type == TokenType::COL ||
        peek().type == TokenType::RECT ||
        peek().type == TokenType::TEXT ||
        peek().type == TokenType::ITEM) {

        Token ahead = tokens[current + 1];
        if (ahead.type == TokenType::LEFT_BRACE) {
            return componentStatement();
        }
        return propertyStatement();
    }

    // For now, advance to avoid infinite loops on unknown tokens
    advance();
    return nullptr;
}

std::unique_ptr<Statement> Parser::componentStatement() {
    Token type = advance(); // Consume the component type
    consume(TokenType::LEFT_BRACE, "Expect '{' after component type.");

    std::vector<std::unique_ptr<Statement>> body;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        body.push_back(statement());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after component body.");

    return std::make_unique<ComponentStmt>(type, std::move(body));
}

std::unique_ptr<Statement> Parser::propertyStatement() {
    Token name = advance(); // Consume the property name
    consume(TokenType::COLON, "Expect ':' after property name.");
    std::unique_ptr<Expression> value = expression();
    return std::make_unique<PropertyStmt>(name, std::move(value));
}

std::unique_ptr<Expression> Parser::expression() {
    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(previous());
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<IdentifierExpr>(previous());
    }

    // If no expression matches, it's an error.
    // For now, return nullptr and let the caller handle it.
    return nullptr;
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
