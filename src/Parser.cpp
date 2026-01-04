#include "Parser.h"
#include "TokenType.h"
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!is_at_end()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (peek().type == TokenType::IDENTIFIER && (current + 1) < tokens.size() && tokens[current + 1].type == TokenType::LEFT_BRACE) {
            return component_declaration();
        }
        return property_declaration();
    } catch (const std::runtime_error& error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::component_declaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect component name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after component name.");
    std::vector<std::unique_ptr<Stmt>> body;
    while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
        body.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after component body.");
    return std::make_unique<ComponentStmt>(name, std::move(body));
}

std::unique_ptr<Stmt> Parser::property_declaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect property name.");
    consume(TokenType::COLON, "Expect ':' after property name.");
    std::unique_ptr<Expr> value = expression();
    return std::make_unique<PropertyStmt>(name, std::move(value));
}

std::unique_ptr<Expr> Parser::expression() {
    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(previous().literal);
    }
    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<IdentifierExpr>(previous());
    }

    throw std::runtime_error("Expect expression.");
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

bool Parser::check(TokenType type) {
    if (is_at_end()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!is_at_end()) current++;
    return previous();
}

bool Parser::is_at_end() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

void Parser::synchronize() {
    advance();
    while (!is_at_end()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::IDENTIFIER:
                return;
        }
        advance();
    }
}
