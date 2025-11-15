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
        std::unique_ptr<TypeExpr> type = nullptr;
        if (match({TokenType::COLON})) {
            type = type_expression();
        }
        consume(TokenType::ASSIGN, "Expect '=' after variable name.");
        std::unique_ptr<Expr> initializer = expression();
        return std::make_unique<LetStmt>(name, std::move(type), std::move(initializer));
    }
    if (match({TokenType::MUT})) {
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
        std::unique_ptr<TypeExpr> type = nullptr;
        if (match({TokenType::COLON})) {
            type = type_expression();
        }
        consume(TokenType::ASSIGN, "Expect '=' after variable name.");
        std::unique_ptr<Expr> initializer = expression();
        return std::make_unique<MutStmt>(name, std::move(type), std::move(initializer));
    }
    if (match({TokenType::FUNC})) {
        return function("function");
    }

    return statement();
}

std::unique_ptr<Stmt> Parser::function(const std::string& kind) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LPAREN, "Expect '(' after " + kind + " name.");
    std::vector<Param> parameters;
    if (peek().type != TokenType::RPAREN) {
        do {
            Token param_name = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            consume(TokenType::COLON, "Expect ':' after parameter name.");
            std::unique_ptr<TypeExpr> param_type = type_expression();
            parameters.push_back({param_name, std::move(param_type)});
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RPAREN, "Expect ')' after parameters.");

    std::unique_ptr<TypeExpr> return_type = nullptr;
    if (match({TokenType::COLON})) {
        return_type = type_expression();
    }

    std::vector<std::unique_ptr<Stmt>> body;
    while (!is_at_end() && peek().type != TokenType::END) {
        body.push_back(declaration());
    }

    consume(TokenType::END, "Expect 'end' after " + kind + " body.");
    return std::make_unique<FunctionStmt>(name, std::move(parameters), std::move(return_type), std::move(body));
}

std::unique_ptr<Stmt> Parser::if_statement() {
    std::unique_ptr<Expr> condition = expression();
    std::unique_ptr<Stmt> then_branch = block();
    std::unique_ptr<Stmt> else_branch = nullptr;
    if (match({TokenType::ELSE})) {
        if (peek().type == TokenType::IF) {
            advance();
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
    if (match({TokenType::FOR})) {
        return for_statement();
    }
    if (match({TokenType::DO})) {
        return do_while_statement();
    }
    if (match({TokenType::RETURN})) {
        return return_statement();
    }
    std::unique_ptr<Expr> expr = expression();
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::return_statement() {
    Token keyword = tokens_[current_ - 1];
    std::unique_ptr<Expr> value = nullptr;
    if (peek().type != TokenType::END) {
        value = expression();
    }
    return std::make_unique<ReturnStmt>(keyword, std::move(value));
}

std::unique_ptr<Stmt> Parser::while_statement() {
    std::unique_ptr<Expr> condition = expression();
    std::unique_ptr<Stmt> body = block();
    consume(TokenType::END, "Expect 'end' after while statement.");
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::do_while_statement() {
    std::unique_ptr<Stmt> body = block();
    consume(TokenType::WHILE, "Expect 'while' after do-while body.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::END, "Expect 'end' after do-while condition.");
    return std::make_unique<DoWhileStmt>(std::move(body), std::move(condition));
}

std::unique_ptr<Stmt> Parser::for_statement() {
    // A bit of a hacky way to check for for-each loop
    // A for-each loop is `for <var> : <container>`
    // A C-style for loop is `for <initializer>; <condition>; <increment>`
    if (peek().type == TokenType::IDENTIFIER && tokens_[current_ + 1].type == TokenType::COLON) {
        Token variable = consume(TokenType::IDENTIFIER, "Expect variable name.");
        consume(TokenType::COLON, "Expect ':' after variable name.");
        std::unique_ptr<Expr> container = expression();
        std::unique_ptr<Stmt> body = block();
        consume(TokenType::END, "Expect 'end' after for loop.");
        return std::make_unique<ForEachStmt>(variable, std::move(container), std::move(body));
    }

    std::unique_ptr<Stmt> initializer;
    if (match({TokenType::LET})) {
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
        std::unique_ptr<TypeExpr> type = nullptr;
        if (match({TokenType::COLON})) {
            type = type_expression();
        }
        consume(TokenType::ASSIGN, "Expect '=' after variable name.");
        std::unique_ptr<Expr> init_expr = expression();
        initializer = std::make_unique<LetStmt>(name, std::move(type), std::move(init_expr));
    } else {
        initializer = std::make_unique<ExpressionStmt>(expression());
    }

    consume(TokenType::SEMICOLON, "Expect ';' after for initializer.");

    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after for condition.");

    std::unique_ptr<Expr> increment = expression();

    std::unique_ptr<Stmt> body = block();
    consume(TokenType::END, "Expect 'end' after for loop.");

    return std::make_unique<ForStmt>(std::move(initializer), std::move(condition), std::move(increment), std::move(body));
}

std::unique_ptr<Stmt> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!is_at_end() && peek().type != TokenType::END && peek().type != TokenType::ELSE && peek().type != TokenType::WHILE) {
        statements.push_back(declaration());
    }
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    std::unique_ptr<Expr> expr = equality();

    if (match({TokenType::ASSIGN})) {
        Token equals = tokens_[current_ - 1];
        std::unique_ptr<Expr> value = assignment();

        if (auto* var = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(var->name, std::move(value));
        }

        throw std::runtime_error("Invalid assignment target.");
    }

    return expr;
}

std::unique_ptr<TypeExpr> Parser::type_expression() {
    Token type_name = consume(TokenType::IDENTIFIER, "Expect type name.");
    std::unique_ptr<TypeExpr> type = std::make_unique<TypeExpr>(type_name);

    if (match({TokenType::LBRACKET})) {
        std::unique_ptr<Expr> size = nullptr;
        if (peek().type != TokenType::RBRACKET) {
            size = expression();
        }
        consume(TokenType::RBRACKET, "Expect ']' after array size.");
        return std::make_unique<ArrayTypeExpr>(std::move(type), std::move(size));
    }

    return type;
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
    return call();
}

std::unique_ptr<Expr> Parser::call() {
    std::unique_ptr<Expr> expr = primary();

    while (true) {
        if (match({TokenType::LPAREN})) {
            expr = finish_call(std::move(expr));
        } else if (match({TokenType::LBRACKET})) {
            expr = finish_subscript(std::move(expr));
        } else {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::TRUE, TokenType::FALSE, TokenType::INTEGER, TokenType::FLOAT, TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(tokens_[current_ - 1]);
    }

    if (peek().type == TokenType::LPAREN && tokens_[current_ + 1].type == TokenType::IDENTIFIER && tokens_[current_ + 2].type == TokenType::COLON) {
        return lambda_expression();
    }

    if (match({TokenType::LPAREN})) {
        std::unique_ptr<Expr> expr = expression();
        consume(TokenType::RPAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    if (match({TokenType::LBRACKET})) {
        Token bracket = tokens_[current_ - 1];
        std::vector<std::unique_ptr<Expr>> elements;
        if (peek().type != TokenType::RBRACKET) {
            do {
                elements.push_back(expression());
            } while (match({TokenType::COMMA}));
        }
        consume(TokenType::RBRACKET, "Expect ']' after array elements.");
        return std::make_unique<ArrayLiteralExpr>(bracket, std::move(elements));
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<VariableExpr>(tokens_[current_ - 1]);
    }

    throw std::runtime_error("Expect expression.");
}

std::unique_ptr<Expr> Parser::lambda_expression() {
    consume(TokenType::LPAREN, "Expect '(' before lambda parameters.");
    std::vector<Param> parameters;
    if (peek().type != TokenType::RPAREN) {
        do {
            Token param_name = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            consume(TokenType::COLON, "Expect ':' after parameter name.");
            std::unique_ptr<TypeExpr> param_type = type_expression();
            parameters.push_back({param_name, std::move(param_type)});
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RPAREN, "Expect ')' after lambda parameters.");

    consume(TokenType::ARROW, "Expect '=>' after lambda parameters.");

    std::unique_ptr<Expr> body = expression();

    return std::make_unique<LambdaExpr>(std::move(parameters), std::move(body));
}

std::unique_ptr<Expr> Parser::finish_call(std::unique_ptr<Expr> callee) {
    std::vector<std::unique_ptr<Expr>> arguments;
    if (peek().type != TokenType::RPAREN) {
        do {
            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }
    Token paren = consume(TokenType::RPAREN, "Expect ')' after arguments.");
    return std::make_unique<CallExpr>(std::move(callee), paren, std::move(arguments));
}

std::unique_ptr<Expr> Parser::finish_subscript(std::unique_ptr<Expr> callee) {
    std::unique_ptr<Expr> index = expression();
    Token bracket = consume(TokenType::RBRACKET, "Expect ']' after index.");
    return std::make_unique<SubscriptExpr>(std::move(callee), bracket, std::move(index));
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
