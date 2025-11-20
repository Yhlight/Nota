#include "Parser.h"
#include "Expr.h"
#include <memory>
#include <string>
#include <iostream>

namespace nota {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<Expr> Parser::parse() {
    try {
        return expression();
    } catch (...) {
        return nullptr;
    }
}

void Parser::error(const std::string& message) {
    std::cerr << "Parse error: " << message << std::endl;
    throw std::runtime_error(message);
}

std::unique_ptr<Expr> Parser::expression() {
    return equality();
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = term();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = unary();
    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        auto right = unary();
        return std::make_unique<Unary>(op, std::move(right));
    }
    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE})) return std::make_unique<Literal>(false, previous().line);
    if (match({TokenType::TRUE})) return std::make_unique<Literal>(true, previous().line);
    if (match({TokenType::NIL})) return std::make_unique<Literal>(nullptr, previous().line);

    if (match({TokenType::NUMBER})) {
        return std::make_unique<Literal>(std::stod(previous().lexeme), previous().line);
    }

    if (match({TokenType::STRING})) {
        const std::string& lexeme = previous().lexeme;
        return std::make_unique<Literal>(lexeme.substr(1, lexeme.length() - 2), previous().line);
    }

    if (match({TokenType::LEFT_PAREN})) {
        auto expr = expression();
        if (!match({TokenType::RIGHT_PAREN})) {
            error("Expect ')' after expression.");
        }
        return std::make_unique<Grouping>(std::move(expr));
    }

    error("Expect expression.");
    return nullptr;
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

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
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

} // namespace nota
