#include "parser.hpp"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!is_at_end()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
    if (match({TokenType::LET})) {
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
        consume(TokenType::ASSIGN, "Expect '=' after variable name.");
        std::unique_ptr<Expr> initializer = expression();
        return std::make_unique<LetStmt>(name, std::move(initializer));
    }
    if (match({TokenType::MUT})) {
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
        consume(TokenType::ASSIGN, "Expect '=' after variable name.");
        std::unique_ptr<Expr> initializer = expression();
        return std::make_unique<MutStmt>(name, std::move(initializer));
    }

    return statement();
}

std::unique_ptr<Stmt> Parser::if_statement() {
    std::unique_ptr<Expr> condition = expression();
    std::unique_ptr<Stmt> then_branch = block();
    std::unique_ptr<Stmt> else_branch = nullptr;
    if (match({TokenType::ELSE})) {
        // Handle 'else if' as a nested if statement
        if (peek().type == TokenType::IF) {
            advance(); // Consume the 'if'
            else_branch = if_statement();
        } else {
            else_branch = block();
        }
    }
    consume(TokenType::END, "Expect 'end' after if statement.");
    return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::IF})) {
        return if_statement();
    }
    if (match({TokenType::WHILE})) {
        return while_statement();
    }
    std::unique_ptr<Expr> expr = expression();
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::while_statement() {
    std::unique_ptr<Expr> condition = expression();
    std::unique_ptr<Stmt> body = block();
    consume(TokenType::END, "Expect 'end' after while statement.");
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!is_at_end() && peek().type != TokenType::END && peek().type != TokenType::ELSE) {
        statements.push_back(declaration());
    }
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Expr> Parser::expression() {
    return equality();
}

std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> expr = comparison();
    while (match({TokenType::EQUAL, TokenType::NOT_EQUAL})) {
        Token op = tokens_[current_ - 1];
        std::unique_ptr<Expr> right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> expr = term();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = tokens_[current_ - 1];
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = tokens_[current_ - 1];
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();
    while (match({TokenType::STAR, TokenType::SLASH})) {
        Token op = tokens_[current_ - 1];
        std::unique_ptr<Expr> right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({TokenType::MINUS})) {
        Token op = tokens_[current_ - 1];
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::TRUE, TokenType::FALSE, TokenType::INTEGER, TokenType::FLOAT, TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(tokens_[current_ - 1]);
    }

    if (match({TokenType::LPAREN})) {
        std::unique_ptr<Expr> expr = expression();
        consume(TokenType::RPAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<VariableExpr>(tokens_[current_ - 1]);
    }

    throw std::runtime_error("Expect expression.");
}


bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (!is_at_end() && peek().type == type) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (!is_at_end() && peek().type == type) {
        return advance();
    }
    throw std::runtime_error(message);
}

bool Parser::is_at_end() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::advance() {
    if (!is_at_end()) {
        current_++;
    }
    return tokens_[current_ - 1];
}

Token Parser::peek() {
    return tokens_[current_];
}
