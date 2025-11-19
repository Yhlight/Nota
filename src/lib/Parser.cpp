#include "Parser.h"
#include <vector>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::shared_ptr<Stmt>> Parser::parse() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::shared_ptr<Stmt> Parser::declaration() {
    try {
        if (match({TokenType::FUNC})) return function("function");
        if (match({TokenType::LET})) return varDeclaration(false);
        if (match({TokenType::MUT})) return varDeclaration(true);
        return statement();
    } catch (const std::runtime_error& e) {
        synchronize();
        return nullptr;
    }
}

std::shared_ptr<Stmt> Parser::varDeclaration(bool isMutable) {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::shared_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    if (peek().type == TokenType::NEWLINE) {
        consume(TokenType::NEWLINE, "Expect newline after variable declaration.");
    }

    return std::make_shared<VarStmt>(name, initializer, isMutable);
}

std::shared_ptr<Stmt> Parser::statement() {
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::FOR})) return forStatement();
    if (match({TokenType::DO})) return doWhileStatement();
    if (match({TokenType::RETURN})) return returnStatement();
    return expressionStatement();
}

std::shared_ptr<Stmt> Parser::function(const std::string& kind) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 255) {
                error(peek(), "Can't have more than 255 parameters.");
            }
            parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    consume(TokenType::NEWLINE, "Expect newline after " + kind + " signature.");
    std::vector<std::shared_ptr<Stmt>> body = block();
    consume(TokenType::END, "Expect 'end' after " + kind + " body.");
    consume(TokenType::NEWLINE, "Expect newline after 'end'.");

    return std::make_shared<FunctionStmt>(name, parameters, body);
}

std::shared_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::shared_ptr<Expr> value = nullptr;
    if (!check(TokenType::NEWLINE)) {
        value = expression();
    }
    consume(TokenType::NEWLINE, "Expect newline after return value.");
    return std::make_shared<ReturnStmt>(keyword, value);
}

std::shared_ptr<Stmt> Parser::ifStatement() {
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::NEWLINE, "Expect newline after if condition.");

    std::vector<std::shared_ptr<Stmt>> thenBranchStmts = block();
    std::shared_ptr<Stmt> elseBranch = nullptr;

    if (match({TokenType::ELSE})) {
        consume(TokenType::NEWLINE, "Expect newline after else.");
        std::vector<std::shared_ptr<Stmt>> elseBranchStmts = block();
        elseBranch = std::make_shared<BlockStmt>(elseBranchStmts);
    }

    consume(TokenType::END, "Expect 'end' after if statement.");
    consume(TokenType::NEWLINE, "Expect newline after end.");

    return std::make_shared<IfStmt>(condition, std::make_shared<BlockStmt>(thenBranchStmts), elseBranch);
}

std::shared_ptr<Stmt> Parser::whileStatement() {
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::NEWLINE, "Expect newline after while condition.");

    std::vector<std::shared_ptr<Stmt>> bodyStmts = block();

    consume(TokenType::END, "Expect 'end' after while statement.");
    consume(TokenType::NEWLINE, "Expect newline after end.");

    return std::make_shared<WhileStmt>(condition, std::make_shared<BlockStmt>(bodyStmts));
}

std::shared_ptr<Stmt> Parser::forStatement() {
    std::shared_ptr<Stmt> initializer;
    if (match({TokenType::LET})) {
        initializer = varDeclaration(false);
    } else if (match({TokenType::MUT})) {
        initializer = varDeclaration(true);
    } else if (!check(TokenType::SEMICOLON)) {
        initializer = expressionStatement();
    } else {
        initializer = nullptr;
    }

    consume(TokenType::SEMICOLON, "Expect ';' after loop initializer.");

    std::shared_ptr<Expr> condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    std::shared_ptr<Expr> increment = nullptr;
    if (!check(TokenType::NEWLINE)) {
        increment = expression();
    }
    consume(TokenType::NEWLINE, "Expect newline after for clauses.");

    std::shared_ptr<Stmt> body = std::make_shared<BlockStmt>(block());

    consume(TokenType::END, "Expect 'end' after for statement.");
    consume(TokenType::NEWLINE, "Expect newline after end.");

    return std::make_shared<ForStmt>(initializer, condition, increment, body);
}

std::shared_ptr<Stmt> Parser::doWhileStatement() {
    consume(TokenType::NEWLINE, "Expect newline after 'do'.");

    std::vector<std::shared_ptr<Stmt>> bodyStmts;
    while (!check(TokenType::WHILE) && !isAtEnd()) {
        bodyStmts.push_back(declaration());
    }

    auto body = std::make_shared<BlockStmt>(bodyStmts);

    consume(TokenType::WHILE, "Expect 'while' after do-while body.");

    std::shared_ptr<Expr> condition = expression();
    if (peek().type == TokenType::NEWLINE) {
        consume(TokenType::NEWLINE, "Expect newline after while condition.");
    }

    return std::make_shared<DoWhileStmt>(body, condition);
}

std::shared_ptr<Stmt> Parser::expressionStatement() {
    auto expr = expression();
    if (peek().type == TokenType::NEWLINE) {
        consume(TokenType::NEWLINE, "Expect newline after expression.");
    }
    return std::make_shared<ExpressionStmt>(expr);
}

std::shared_ptr<Expr> Parser::expression() {
    return assignment();
}

std::shared_ptr<Expr> Parser::assignment() {
    auto expr = equality();

    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        auto value = assignment();

        if (auto var = std::dynamic_pointer_cast<Variable>(expr)) {
            return std::make_shared<Assign>(var->name, value);
        }

        error(equals, "Invalid assignment target.");
    }

    return expr;
}

std::shared_ptr<Expr> Parser::equality() {
    auto expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
    auto expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        auto right = term();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::term() {
    auto expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        auto right = factor();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::factor() {
    auto expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        auto right = unary();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        auto right = unary();
        return std::make_shared<Unary>(op, right);
    }

    return postfix();
}

std::shared_ptr<Expr> Parser::finishCall(std::shared_ptr<Expr> callee) {
    std::vector<std::shared_ptr<Expr>> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                error(peek(), "Can't have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_shared<Call>(callee, paren, arguments);
}

std::shared_ptr<Expr> Parser::postfix() {
    auto expr = primary();

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(expr);
        } else if (match({TokenType::PLUS_PLUS, TokenType::MINUS_MINUS})) {
            Token op = previous();
            expr = std::make_shared<Postfix>(expr, op);
        } else {
            break;
        }
    }

    return expr;
}

std::shared_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE})) return std::make_shared<Literal>(false);
    if (match({TokenType::TRUE})) return std::make_shared<Literal>(true);
    if (match({TokenType::NONE})) return std::make_shared<Literal>(std::monostate{});

    if (match({TokenType::NUMBER_INT, TokenType::NUMBER_FLOAT, TokenType::STRING})) {
        return std::make_shared<Literal>(previous().literal);
    }

    if (match({TokenType::LEFT_PAREN})) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Grouping>(expr);
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_shared<Variable>(previous());
    }

    throw error(peek(), "Expect expression.");
}

std::vector<std::shared_ptr<Stmt>> Parser::block() {
    std::vector<std::shared_ptr<Stmt>> statements;

    while (!check(TokenType::END) && !check(TokenType::ELSE) && !isAtEnd()) {
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

bool Parser::match(const std::vector<TokenType>& types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

std::runtime_error Parser::error(const Token& token, const std::string& message) {
    errors.push_back({token.line, message});
    return std::runtime_error(message);
}

bool Parser::hasError() const {
    return !errors.empty();
}

const std::vector<NotaError>& Parser::getErrors() const {
    return errors;
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::NEWLINE) return;

        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FUNC:
            case TokenType::LET:
            case TokenType::MUT:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
        }

        advance();
    }
}
