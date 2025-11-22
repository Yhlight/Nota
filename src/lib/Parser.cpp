#include "Parser.h"
#include <stdexcept>

namespace nota {

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

std::vector<std::shared_ptr<Stmt>> Parser::parse() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        skipNewlines();
        if (isAtEnd()) break;
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
        } else if (auto get = std::dynamic_pointer_cast<GetExpr>(expr)) {
            return std::make_shared<SetExpr>(get->object, get->name, value);
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
    return call();
}

std::shared_ptr<Expr> Parser::call() {
    std::shared_ptr<Expr> expr = primary();

    while (true) {
        if (match({TokenType::LPAREN})) {
            expr = finishCall(expr);
        } else if (match({TokenType::DOT})) {
            Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            expr = std::make_shared<GetExpr>(expr, name);
        } else if (match({TokenType::PLUS_PLUS})) {
            Token op = previous();
            expr = std::make_shared<Postfix>(expr, op);
        } else {
            break;
        }
    }

    return expr;
}

std::shared_ptr<Expr> Parser::finishCall(std::shared_ptr<Expr> callee) {
    std::vector<std::shared_ptr<Expr>> arguments;
    if (peek().type != TokenType::RPAREN) {
        do {
            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RPAREN, "Expect ')' after arguments.");

    return std::make_shared<CallExpr>(callee, paren, arguments);
}

std::shared_ptr<Expr> Parser::primary() {
    if (match({TokenType::TRUE})) {
        return std::make_shared<Literal>(true);
    }
    if (match({TokenType::FALSE})) {
        return std::make_shared<Literal>(false);
    }

    if (match({TokenType::THIS})) {
        return std::make_shared<ThisExpr>(previous());
    }

    if (match({TokenType::INTEGER, TokenType::FLOAT, TokenType::STRING})) {
        return std::make_shared<Literal>(previous().literal);
    }

    if (match({TokenType::IDENTIFIER})) {
        Token identifier = previous();
        if (match({TokenType::COLON_COLON})) {
            Token member = consume(TokenType::IDENTIFIER, "Expect member name after '::'.");
            return std::make_shared<ModuleAccessExpr>(identifier, member);
        }
        return std::make_shared<Variable>(identifier);
    }

    if (match({TokenType::LPAREN})) {
        // This could be a grouped expression or a lambda.
        // To decide, we need to find the matching right parenthesis and see if it's followed by an arrow.
        int paren_count = 1;
        size_t lookahead_pos = current_;
        while (paren_count > 0 && lookahead_pos < tokens_.size()) {
            if (tokens_[lookahead_pos].type == TokenType::LPAREN) paren_count++;
            if (tokens_[lookahead_pos].type == TokenType::RPAREN) paren_count--;
            lookahead_pos++;
        }
        if (lookahead_pos < tokens_.size() && tokens_[lookahead_pos].type == TokenType::ARROW) {
             // It's a lambda
            std::vector<Token> parameters;
            if (peek().type != TokenType::RPAREN) {
                do {
                    parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
                } while (match({TokenType::COMMA}));
            }
            consume(TokenType::RPAREN, "Expect ')' after parameters.");
            consume(TokenType::ARROW, "Expect '=>' after lambda parameters.");

            std::vector<std::shared_ptr<Stmt>> body;
            if (match({TokenType::NEWLINE})) {
                // Multi-line lambda
                body = block();
                consume(TokenType::END, "Expect 'end' after lambda body.");
            } else {
                // Single-line lambda
                std::shared_ptr<Expr> expr = expression();
                body.push_back(std::make_shared<ReturnStmt>(Token{TokenType::RETURN, "return", {}, peek().line}, expr));
            }
            return std::make_shared<LambdaExpr>(parameters, body);
        } else {
            // It's a grouped expression
            std::shared_ptr<Expr> expr = expression();
            consume(TokenType::RPAREN, "Expect ')' after expression.");
            return std::make_shared<Grouping>(expr);
        }
    }

    throw error(peek(), "Expect expression.");
}

std::shared_ptr<Stmt> Parser::declaration() {
    if (match({TokenType::CLASS})) {
        return classDeclaration();
    }
    if (match({TokenType::FN})) {
        return functionDeclaration();
    }
    if (match({TokenType::IMPORT})) {
        return importStatement();
    }
    if (match({TokenType::PACKAGE})) {
        return packageStatement();
    }
    if (match({TokenType::LET, TokenType::MUT})) {
        return varDeclaration();
    }
    return statement();
}

std::shared_ptr<Stmt> Parser::classDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect class name.");

    std::vector<std::shared_ptr<FunctionStmt>> methods;
    while (peek().type != TokenType::END && !isAtEnd()) {
        if (match({TokenType::NEWLINE})) {
            // allow newlines between methods
            continue;
        }
        if (match({TokenType::FN})) {
            methods.push_back(std::dynamic_pointer_cast<FunctionStmt>(functionDeclaration()));
        } else {
            break;
        }
    }

    consume(TokenType::END, "Expect 'end' after class body.");
    return std::make_shared<ClassStmt>(name, methods);
}

std::shared_ptr<Stmt> Parser::importStatement() {
    Token path = consume(TokenType::STRING, "Expect module path.");
    std::optional<Token> alias;
    if (match({TokenType::AS})) {
        alias = consume(TokenType::IDENTIFIER, "Expect alias name.");
    }
    consumeTerminators();
    return std::make_shared<ImportStmt>(path, alias);
}

std::shared_ptr<Stmt> Parser::packageStatement() {
    Token name = consume(TokenType::IDENTIFIER, "Expect package name.");
    consumeTerminators();
    return std::make_shared<PackageStmt>(name);
}

std::shared_ptr<Stmt> Parser::functionDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
    consume(TokenType::LPAREN, "Expect '(' after function name.");
    std::vector<Token> parameters;
    if (peek().type != TokenType::RPAREN) {
        do {
            parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RPAREN, "Expect ')' after parameters.");

    std::vector<std::shared_ptr<Stmt>> body = block();
    consume(TokenType::END, "Expect 'end' after function body.");
    return std::make_shared<FunctionStmt>(name, parameters, body);
}

std::shared_ptr<Stmt> Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::shared_ptr<Expr> initializer = nullptr;
    if (match({TokenType::ASSIGN})) {
        initializer = expression();
    }

    consumeTerminators();
    return std::make_shared<VarStmt>(name, initializer);
}

std::shared_ptr<Stmt> Parser::statement() {
    if (match({TokenType::IF})) {
        return ifStatement();
    }
    if (match({TokenType::WHILE})) {
        return whileStatement();
    }
    if (match({TokenType::FOR})) {
        return forStatement();
    }
    if (match({TokenType::DO})) {
        return doWhileStatement();
    }
    if (match({TokenType::RETURN})) {
        return returnStatement();
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

std::shared_ptr<Stmt> Parser::forStatement() {
    // Initializer
    std::shared_ptr<Stmt> initializer;
    if (match({TokenType::SEMICOLON})) {
        initializer = nullptr;
    } else if (peek().type == TokenType::LET || peek().type == TokenType::MUT) {
        // It's a variable declaration, but we handle the semicolon here.
        advance(); // consume 'let' or 'mut'
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
        std::shared_ptr<Expr> init_expr = nullptr;
        if (match({TokenType::ASSIGN})) {
            init_expr = expression();
        }
        initializer = std::make_shared<VarStmt>(name, init_expr);
        consume(TokenType::SEMICOLON, "Expect ';' after for loop initializer.");
    } else {
        // It's an expression statement, but we handle the semicolon here.
        std::shared_ptr<Expr> expr = expression();
        initializer = std::make_shared<ExpressionStmt>(expr);
        consume(TokenType::SEMICOLON, "Expect ';' after for loop initializer.");
    }

    // Condition
    std::shared_ptr<Expr> condition = nullptr;
    if (peek().type != TokenType::SEMICOLON) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    // Increment
    std::shared_ptr<Expr> increment = nullptr;
    if (peek().type != TokenType::END) {
        increment = expression();
    }

    // Body
    std::shared_ptr<Stmt> body = std::make_shared<Block>(block());
    consume(TokenType::END, "Expect 'end' after for clauses.");

    // Desugar into a while loop
    if (increment != nullptr) {
        std::vector<std::shared_ptr<Stmt>> statements = { body, std::make_shared<ExpressionStmt>(increment) };
        body = std::make_shared<Block>(statements);
    }

    if (condition == nullptr) {
        condition = std::make_shared<Literal>(true);
    }
    body = std::make_shared<WhileStmt>(condition, body);

    if (initializer != nullptr) {
        std::vector<std::shared_ptr<Stmt>> statements = { initializer, body };
        body = std::make_shared<Block>(statements);
    }

    return body;
}

std::shared_ptr<Stmt> Parser::doWhileStatement() {
    std::shared_ptr<Stmt> body = std::make_shared<Block>(block());
    consume(TokenType::WHILE, "Expect 'while' after do-while body.");
    std::shared_ptr<Expr> condition = expression();
    consumeTerminators();
    return std::make_shared<DoWhileStmt>(body, condition);
}

std::shared_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::shared_ptr<Expr> value = nullptr;
    if (peek().type != TokenType::NEWLINE && peek().type != TokenType::END_OF_FILE && peek().type != TokenType::SEMICOLON) {
        value = expression();
    }

    consumeTerminators();
    return std::make_shared<ReturnStmt>(keyword, value);
}


std::vector<std::shared_ptr<Stmt>> Parser::block() {
    std::vector<std::shared_ptr<Stmt>> statements;

    while (peek().type != TokenType::END && peek().type != TokenType::ELSE && peek().type != TokenType::WHILE && !isAtEnd()) {
        skipNewlines();
        if (peek().type == TokenType::END || peek().type == TokenType::ELSE || peek().type == TokenType::WHILE || isAtEnd()) break;
        statements.push_back(declaration());
    }

    return statements;
}


std::shared_ptr<Stmt> Parser::expressionStatement() {
    std::shared_ptr<Expr> expr = expression();
    consumeTerminators();
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

void Parser::consumeTerminators() {
    if (match({TokenType::SEMICOLON})) {
        // Consume an optional semicolon.
        return;
    }

    bool consumed_newline = false;
    while (peek().type == TokenType::NEWLINE) {
        advance();
        consumed_newline = true;
    }

    if (!consumed_newline && !isAtEnd()) {
        throw error(peek(), "Expect newline or ';' after statement.");
    }
}

void Parser::skipNewlines() {
    while (peek().type == TokenType::NEWLINE) {
        advance();
    }
}

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    // In a real implementation, this would likely print to stderr
    // and synchronize the parser.
    return ParseError(message);
}


} // namespace nota
