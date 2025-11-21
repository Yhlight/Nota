#include "Parser.h"
#include <stdexcept>

namespace nota {

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

std::vector<std::shared_ptr<Stmt>> Parser::parse() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::shared_ptr<Expr> Parser::expression() {
    return assignment();
}

std::shared_ptr<Expr> Parser::assignment() {
    std::shared_ptr<Expr> expr = equality();

    if (match({TokenType::ASSIGN})) {
        Token equals = previous();
        std::shared_ptr<Expr> value = assignment();

        if (auto var = std::dynamic_pointer_cast<Variable>(expr)) {
            Token name = var->name;
            return std::make_shared<Assign>(name, value);
        }

        error(equals, "Invalid assignment target.");
    }

    return expr;
}

std::shared_ptr<Expr> Parser::equality() {
    std::shared_ptr<Expr> expr = comparison();

    while (match({TokenType::NOT_EQUALS, TokenType::EQUALS})) {
        Token op = previous();
        std::shared_ptr<Expr> right = comparison();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
    std::shared_ptr<Expr> expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUALS, TokenType::LESS, TokenType::LESS_EQUALS})) {
        Token op = previous();
        std::shared_ptr<Expr> right = term();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::term() {
    std::shared_ptr<Expr> expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::shared_ptr<Expr> right = factor();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::factor() {
    std::shared_ptr<Expr> expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::unary() {
    if (match({TokenType::NOT, TokenType::MINUS})) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<Unary>(op, right);
    }
    return primary();
}

std::shared_ptr<Expr> Parser::primary() {
    if (match({TokenType::TRUE})) {
        return std::make_shared<Literal>(true);
    }
    if (match({TokenType::FALSE})) {
        return std::make_shared<Literal>(false);
    }

    if (match({TokenType::INTEGER, TokenType::FLOAT, TokenType::STRING})) {
        return std::make_shared<Literal>(previous().literal);
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_shared<Variable>(previous());
    }

    if (match({TokenType::LPAREN})) {
        std::shared_ptr<Expr> expr = expression();
        consume(TokenType::RPAREN, "Expect ')' after expression.");
        return std::make_shared<Grouping>(expr);
    }

    throw error(peek(), "Expect expression.");
}

std::shared_ptr<Stmt> Parser::declaration() {
    if (match({TokenType::LET, TokenType::MUT})) {
        return varDeclaration();
    }
    return statement();
}

std::shared_ptr<Stmt> Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::shared_ptr<Expr> initializer = nullptr;
    if (match({TokenType::ASSIGN})) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<VarStmt>(name, initializer);
}

std::shared_ptr<Stmt> Parser::statement() {
    if (match({TokenType::IF})) {
        return ifStatement();
    }
    if (match({TokenType::WHILE})) {
        return whileStatement();
    }
    return expressionStatement();
}

std::shared_ptr<Stmt> Parser::whileStatement() {
    std::shared_ptr<Expr> condition = expression();
    std::shared_ptr<Stmt> body = std::make_shared<Block>(block());
    consume(TokenType::END, "Expect 'end' after while statement.");
    return std::make_shared<WhileStmt>(condition, body);
}

std::shared_ptr<Stmt> Parser::ifStatement() {
    std::shared_ptr<Expr> condition = expression();
    std::shared_ptr<Stmt> thenBranch = std::make_shared<Block>(block());
    std::shared_ptr<Stmt> elseBranch = nullptr;

    // The 'else' branch is parsed as a single statement to allow for 'else if' chaining.
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    consume(TokenType::END, "Expect 'end' after if statement.");
    return std::make_shared<IfStmt>(condition, thenBranch, elseBranch);
}

std::vector<std::shared_ptr<Stmt>> Parser::block() {
    std::vector<std::shared_ptr<Stmt>> statements;

    while (peek().type != TokenType::END && peek().type != TokenType::ELSE && !isAtEnd()) {
        statements.push_back(declaration());
    }

    return statements;
}


std::shared_ptr<Stmt> Parser::expressionStatement() {
    std::shared_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<ExpressionStmt>(expr);
}


bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (peek().type == type) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() const {
    return tokens_[current_];
}

Token Parser::previous() const {
    return tokens_[current_ - 1];
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (peek().type == type) return advance();
    throw error(peek(), message);
}

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    // In a real implementation, this would likely print to stderr
    // and synchronize the parser.
    return ParseError(message);
}


} // namespace nota
