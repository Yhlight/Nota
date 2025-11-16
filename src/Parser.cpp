#include "Parser.h"

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

// Helper methods
Token Parser::peek() const {
    return tokens[position];
}

Token Parser::advance() {
    return tokens[position++];
}

bool Parser::is_at_end() const {
    return position >= tokens.size();
}

// Parsing methods

std::unique_ptr<Stmt> Parser::parse_statement() {
    if (peek().type == TokenType::Let) {
        return parse_let_statement();
    }
    // Handle other statement types here...
    return nullptr;
}

std::unique_ptr<Stmt> Parser::parse_let_statement() {
    advance(); // Consume 'let'

    if (peek().type != TokenType::Identifier) {
        // Error handling: expected an identifier
        return nullptr;
    }
    Token name = advance();

    if (peek().type != TokenType::Equal) {
        // Error handling: expected an '='
        return nullptr;
    }
    advance(); // Consume '='

    std::unique_ptr<Expr> initializer = parse_expression();
    if (!initializer) {
        // Error handling: expected an expression
        return nullptr;
    }

    return std::make_unique<VarDeclStmt>(name, std::move(initializer));
}

std::unique_ptr<Expr> Parser::parse_primary_expression() {
    if (peek().type == TokenType::Number) {
        return std::make_unique<NumberExpr>(advance());
    }
    // Handle other primary expression types here...
    return nullptr;
}

std::unique_ptr<Expr> Parser::parse_expression() {
    std::unique_ptr<Expr> left = parse_primary_expression();

    while (!is_at_end() && peek().type == TokenType::Plus) {
        Token op = advance();
        std::unique_ptr<Expr> right = parse_primary_expression();
        left = std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
    }

    return left;
}

Program Parser::parse() {
    Program program;
    while (!is_at_end()) {
        std::unique_ptr<Stmt> stmt = parse_statement();
        if (stmt) {
            program.push_back(std::move(stmt));
        } else {
            // For now, skip tokens that don't form a valid statement
            advance();
        }
    }
    return program;
}
