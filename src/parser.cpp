#include "parser.hpp"
#include "token.hpp"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::shared_ptr<Stmt>> Parser::parse() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
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

template<typename... Args>
bool Parser::match(Args... types) {
    for (TokenType type : {types...}) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

std::shared_ptr<Stmt> Parser::declaration() {
    try {
        if (match(TokenType::LET, TokenType::MUT)) return varDeclaration();
        return statement();
    } catch (ParseError& error) {
        synchronize();
        return nullptr;
    }
}

std::shared_ptr<Stmt> Parser::statement() {
    if (match(TokenType::IF)) {
        std::shared_ptr<Stmt> stmt = ifStatement();
        consume(TokenType::END, "Expect 'end' after if statement.");
        return stmt;
    }
    if (match(TokenType::WHILE)) return whileStatement();
    return expressionStatement();
}

std::vector<std::shared_ptr<Stmt>> Parser::block() {
    std::vector<std::shared_ptr<Stmt>> statements;

    while (!check(TokenType::END) && !check(TokenType::ELSE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    return statements;
}

std::shared_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    std::shared_ptr<Stmt> thenBranch = std::make_shared<BlockStmt>(block());
    std::shared_ptr<Stmt> elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
        if (match(TokenType::IF)) {
            elseBranch = ifStatement();
        } else {
            elseBranch = std::make_shared<BlockStmt>(block());
        }
    }

    return std::make_shared<IfStmt>(condition, thenBranch, elseBranch);
}

std::shared_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
    std::shared_ptr<Stmt> body = std::make_shared<BlockStmt>(block());

    consume(TokenType::END, "Expect 'end' after while statement.");
    return std::make_shared<WhileStmt>(condition, body);
}

std::shared_ptr<Stmt> Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::shared_ptr<Expr> initializer = nullptr;
    if (match(TokenType::EQUAL)) {
        initializer = expression();
    }

    return std::make_shared<VarStmt>(name, initializer);
}

std::shared_ptr<Stmt> Parser::expressionStatement() {
    std::shared_ptr<Expr> expr = expression();
    return std::make_shared<ExpressionStmt>(expr);
}

std::shared_ptr<Expr> Parser::expression() {
    return equality();
}

std::shared_ptr<Expr> Parser::equality() {
    std::shared_ptr<Expr> expr = comparison();

    while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
        Token op = previous();
        std::shared_ptr<Expr> right = comparison();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
    std::shared_ptr<Expr> expr = term();

    while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
        Token op = previous();
        std::shared_ptr<Expr> right = term();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::term() {
    std::shared_ptr<Expr> expr = factor();

    while (match(TokenType::MINUS, TokenType::PLUS)) {
        Token op = previous();
        std::shared_ptr<Expr> right = factor();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::factor() {
    std::shared_ptr<Expr> expr = unary();

    while (match(TokenType::SLASH, TokenType::STAR)) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::unary() {
    if (match(TokenType::BANG, TokenType::MINUS)) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<UnaryExpr>(op, right);
    }

    return primary();
}

std::shared_ptr<Expr> Parser::primary() {
    if (match(TokenType::FALSE)) return std::make_shared<LiteralExpr>(false);
    if (match(TokenType::TRUE)) return std::make_shared<LiteralExpr>(true);
    if (match(TokenType::NIL)) return std::make_shared<LiteralExpr>(std::monostate());

    if (match(TokenType::NUMBER)) {
        std::string lexeme = previous().lexeme;
        if (lexeme.find('.') != std::string::npos) {
            return std::make_shared<LiteralExpr>(std::stod(lexeme));
        } else {
            return std::make_shared<LiteralExpr>(std::stoll(lexeme));
        }
    }
    if (match(TokenType::STRING)) {
        return std::make_shared<LiteralExpr>(previous().lexeme);
    }

    if (match(TokenType::LEFT_PAREN)) {
        std::shared_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<GroupingExpr>(expr);
    }

    throw error(peek(), "Expect expression.");
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

ParseError Parser::error(Token token, const std::string& message) {
    if (token.type == TokenType::END_OF_FILE) {
        std::cerr << "Error at end: " << message << std::endl;
    } else {
        std::cerr << "Error at '" << token.lexeme << "': " << message << std::endl;
    }
    return ParseError(message);
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::END_OF_FILE) return;

        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
            case TokenType::END:
                return;
        }

        advance();
    }
}
