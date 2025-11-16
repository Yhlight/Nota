#include "parser.hpp"

namespace nota {

    Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

    std::vector<std::unique_ptr<Stmt>> Parser::parse() {
        std::vector<std::unique_ptr<Stmt>> statements;
        while (!is_at_end()) {
            try {
                statements.push_back(declaration());
            } catch (const ParseError& error) {
                synchronize();
            }
        }
        return statements;
    }

    std::unique_ptr<Stmt> Parser::declaration() {
        if (match({TokenType::CLASS})) return class_declaration();
        if (match({TokenType::FUNC})) return function_declaration();
        if (match({TokenType::LET})) return var_declaration(false);
        if (match({TokenType::MUT})) return var_declaration(true);
        return statement();
    }

    std::unique_ptr<Stmt> Parser::class_declaration() {
        Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
        std::vector<std::unique_ptr<VarStmt>> fields;
        std::vector<std::unique_ptr<FunctionStmt>> methods;

        while (!check(TokenType::END) && !is_at_end()) {
            if (match({TokenType::FUNC})) {
                methods.push_back(std::unique_ptr<FunctionStmt>(static_cast<FunctionStmt*>(function_declaration().release())));
            } else {
                fields.push_back(std::unique_ptr<VarStmt>(static_cast<VarStmt*>(var_declaration(false).release())));
            }
        }

        consume(TokenType::END, "Expect 'end' after class body.");
        return std::make_unique<ClassStmt>(name, std::move(fields), std::move(methods));
    }

    std::unique_ptr<Stmt> Parser::function_declaration() {
        Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
        consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");
        std::vector<Param> parameters;
        if (!check(TokenType::RIGHT_PAREN)) {
            do {
                Token param_name = consume(TokenType::IDENTIFIER, "Expect parameter name.");
                std::unique_ptr<TypeExpr> param_type = nullptr;
                if (match({TokenType::COLON})) {
                    param_type = type();
                }
                parameters.push_back({param_name, std::move(param_type)});
            } while (match({TokenType::COMMA}));
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

        std::unique_ptr<TypeExpr> return_type = nullptr;
        if (match({TokenType::COLON})) {
            return_type = type();
        }

        return std::make_unique<FunctionStmt>(name, std::move(parameters), std::make_unique<BlockStmt>(block()), std::move(return_type));
    }

    std::unique_ptr<Stmt> Parser::var_declaration(bool is_mutable) {
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
        std::unique_ptr<TypeExpr> type_expr = nullptr;
        if (match({TokenType::COLON})) {
            type_expr = type();
        }
        std::unique_ptr<Expr> initializer = nullptr;
        if (match({TokenType::EQUAL})) {
            initializer = expression();
        }
        return std::make_unique<VarStmt>(name, std::move(type_expr), std::move(initializer), is_mutable);
    }

    std::unique_ptr<Stmt> Parser::statement() {
        if (check(TokenType::FOR)) {
            if (is_for_each()) {
                advance(); // consume 'for'
                return for_each_statement();
            }
            return for_statement();
        }
        if (match({TokenType::IF})) return if_statement();
        if (match({TokenType::RETURN})) return return_statement();
        if (match({TokenType::WHILE})) return while_statement();
        if (match({TokenType::DO})) return do_while_statement();
        if (match({TokenType::MATCH})) return match_statement();
        return expression_statement();
    }

    std::unique_ptr<Stmt> Parser::for_statement() {
        std::unique_ptr<Stmt> initializer;
        if (match({TokenType::SEMICOLON})) {
            initializer = nullptr;
        } else if (match({TokenType::LET, TokenType::MUT})) {
            initializer = var_declaration(previous().type == TokenType::MUT);
        } else {
            initializer = expression_statement();
        }
        consume(TokenType::SEMICOLON, "Expect ';' after loop initializer.");

        std::unique_ptr<Expr> condition = nullptr;
        if (!check(TokenType::SEMICOLON)) {
            condition = expression();
        }
        consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

        std::unique_ptr<Expr> increment = nullptr;
        if (!check(TokenType::END)) {
            increment = expression();
        }

        std::unique_ptr<Stmt> body = std::make_unique<BlockStmt>(block());

        // Desugar 'for' loop into a 'while' loop
        if (increment) {
            std::vector<std::unique_ptr<Stmt>> block_stmts;
            block_stmts.push_back(std::move(body));
            block_stmts.push_back(std::make_unique<ExpressionStmt>(std::move(increment)));
            body = std::make_unique<BlockStmt>(std::move(block_stmts));
        }

        if (!condition) {
            condition = std::make_unique<LiteralExpr>("true");
        }
        auto while_loop = std::make_unique<WhileStmt>(std::move(condition), std::move(body));

        if (initializer) {
            std::vector<std::unique_ptr<Stmt>> block_stmts;
            block_stmts.push_back(std::move(initializer));
            block_stmts.push_back(std::move(while_loop));
            return std::make_unique<BlockStmt>(std::move(block_stmts));
        }

        return while_loop;
    }

    std::unique_ptr<Stmt> Parser::for_each_statement() {
        bool is_mutable = match({TokenType::MUT});
        if (!is_mutable) match({TokenType::LET}); // Optional let

        Token variable = consume(TokenType::IDENTIFIER, "Expect variable name in for-each loop.");
        consume(TokenType::COLON, "Expect ':' after variable name in for-each loop.");
        std::unique_ptr<Expr> collection = expression();
        std::unique_ptr<Stmt> body = std::make_unique<BlockStmt>(block());

        return std::make_unique<ForEachStmt>(variable, std::move(collection), std::move(body), is_mutable);
    }

    std::unique_ptr<Stmt> Parser::if_statement() {
        std::unique_ptr<Expr> condition = expression();
        std::unique_ptr<Stmt> then_branch = std::make_unique<BlockStmt>(block());
        std::unique_ptr<Stmt> else_branch = nullptr;
        if (match({TokenType::ELSE})) {
            else_branch = statement();
        }
        return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
    }

    std::unique_ptr<Stmt> Parser::return_statement() {
        Token keyword = previous();
        std::unique_ptr<Expr> value = nullptr;
        if (!check(TokenType::END)) { // Assuming 'end' terminates a block, not return
            value = expression();
        }
        return std::make_unique<ReturnStmt>(keyword, std::move(value));
    }

    std::unique_ptr<Stmt> Parser::while_statement() {
        std::unique_ptr<Expr> condition = expression();
        std::unique_ptr<Stmt> body = std::make_unique<BlockStmt>(block());
        return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
    }

    std::unique_ptr<Stmt> Parser::do_while_statement() {
        std::unique_ptr<Stmt> body = statement();
        consume(TokenType::WHILE, "Expect 'while' after do-while body.");
        std::unique_ptr<Expr> condition = expression();
        return std::make_unique<DoWhileStmt>(std::move(condition), std::move(body));
    }

    std::unique_ptr<Stmt> Parser::match_statement() {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'match'.");
        std::unique_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after match expression.");

        std::vector<MatchCase> cases;
        while (!check(TokenType::END) && !is_at_end()) {
            std::vector<std::unique_ptr<Expr>> patterns;
            do {
                if (match({TokenType::IDENTIFIER}) && previous().lexeme == "_") {
                    patterns.push_back(std::make_unique<LiteralExpr>("_"));
                } else {
                    patterns.push_back(primary());
                }
            } while (match({TokenType::COMMA}));

            consume(TokenType::COLON, "Expect ':' after match case pattern.");
            std::unique_ptr<Stmt> body = std::make_unique<BlockStmt>(block());
            cases.push_back({std::move(patterns), std::move(body)});
        }

        return std::make_unique<MatchStmt>(std::move(expr), std::move(cases));
    }

    std::vector<std::unique_ptr<Stmt>> Parser::block() {
        std::vector<std::unique_ptr<Stmt>> statements;
        while (!check(TokenType::END) && !check(TokenType::ELSE) && !is_at_end()) {
            statements.push_back(declaration());
        }
        consume(TokenType::END, "Expect 'end' after block.");
        return statements;
    }

    std::unique_ptr<Stmt> Parser::braced_block() {
        std::vector<std::unique_ptr<Stmt>> statements;
        while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
            statements.push_back(declaration());
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
        return std::make_unique<BlockStmt>(std::move(statements));
    }


    std::unique_ptr<Stmt> Parser::expression_statement() {
        std::unique_ptr<Expr> expr = expression();
        return std::make_unique<ExpressionStmt>(std::move(expr));
    }


    std::unique_ptr<Expr> Parser::expression() {
        return assignment();
    }

    std::unique_ptr<Expr> Parser::assignment() {
        std::unique_ptr<Expr> expr = logical_or();
        if (match({TokenType::EQUAL})) {
            Token equals = previous();
            std::unique_ptr<Expr> value = assignment();
            if (auto var_expr = dynamic_cast<VariableExpr*>(expr.get())) {
                return std::make_unique<AssignExpr>(var_expr->name, std::move(value));
            } else if (auto get_expr = dynamic_cast<GetExpr*>(expr.get())) {
                return std::make_unique<SetExpr>(std::move(get_expr->object), get_expr->name, std::move(value));
            }
            throw ParseError("Invalid assignment target.");
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::lambda_expression() {
        consume(TokenType::LEFT_PAREN, "Expect '(' before lambda parameters.");
        std::vector<Param> parameters;
        if (!check(TokenType::RIGHT_PAREN)) {
            do {
                Token param_name = consume(TokenType::IDENTIFIER, "Expect parameter name.");
                std::unique_ptr<TypeExpr> param_type = nullptr;
                if (match({TokenType::COLON})) {
                    param_type = type();
                }
                parameters.push_back({param_name, std::move(param_type)});
            } while (match({TokenType::COMMA}));
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
        consume(TokenType::ARROW, "Expect '=>' after lambda parameters.");
        std::unique_ptr<Expr> body = expression();

        return std::make_unique<LambdaExpr>(std::move(parameters), std::move(body), nullptr);
    }

    std::unique_ptr<Expr> Parser::logical_or() {
        std::unique_ptr<Expr> expr = logical_and();
        while (match({TokenType::OR})) {
            Token op = previous();
            std::unique_ptr<Expr> right = logical_and();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::logical_and() {
        std::unique_ptr<Expr> expr = equality();
        while (match({TokenType::AND})) {
            Token op = previous();
            std::unique_ptr<Expr> right = equality();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::equality() {
        std::unique_ptr<Expr> expr = comparison();
        while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
            Token op = previous();
            std::unique_ptr<Expr> right = comparison();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::comparison() {
        std::unique_ptr<Expr> expr = term();
        while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
            Token op = previous();
            std::unique_ptr<Expr> right = term();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::term() {
        std::unique_ptr<Expr> expr = factor();
        while (match({TokenType::MINUS, TokenType::PLUS})) {
            Token op = previous();
            std::unique_ptr<Expr> right = factor();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::factor() {
        std::unique_ptr<Expr> expr = unary();
        while (match({TokenType::SLASH, TokenType::STAR, TokenType::PERCENT})) {
            Token op = previous();
            std::unique_ptr<Expr> right = unary();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::unary() {
        if (match({TokenType::BANG, TokenType::MINUS})) {
            Token op = previous();
            std::unique_ptr<Expr> right = unary();
            return std::make_unique<UnaryExpr>(op, std::move(right));
        }
        return call();
    }

    std::unique_ptr<Expr> Parser::call() {
        std::unique_ptr<Expr> expr = primary();

        while (true) {
            if (match({TokenType::LEFT_PAREN})) {
                std::vector<std::unique_ptr<Expr>> arguments;
                if (!check(TokenType::RIGHT_PAREN)) {
                    do {
                        arguments.push_back(expression());
                    } while (match({TokenType::COMMA}));
                }
                Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
                expr = std::make_unique<CallExpr>(std::move(expr), paren, std::move(arguments));
            } else if (match({TokenType::DOT})) {
                Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
                expr = std::make_unique<GetExpr>(std::move(expr), name);
            } else if (match({TokenType::LEFT_BRACKET})) {
                Token bracket = previous();
                std::unique_ptr<Expr> index = expression();
                consume(TokenType::RIGHT_BRACKET, "Expect ']' after index.");
                expr = std::make_unique<IndexExpr>(std::move(expr), bracket, std::move(index));
            }
            else {
                break;
            }
        }

        return expr;
    }

    bool Parser::is_lambda() {
        if (peek().type != TokenType::LEFT_PAREN) return false;

        // Look for '=>' to confirm it's a lambda
        int lookahead = 1;
        while (tokens[current + lookahead].type != TokenType::EOF_TOKEN) {
            if (tokens[current + lookahead].type == TokenType::ARROW) {
                return true;
            }
            if (tokens[current + lookahead].type == TokenType::RIGHT_PAREN) {
                 // Check for arrow after the closing paren
                if (tokens[current + lookahead + 1].type == TokenType::ARROW) {
                    return true;
                } else {
                    return false;
                }
            }
            lookahead++;
        }
        return false;
    }

    bool Parser::is_for_each() {
        // This is called after 'for' is checked
        int lookahead = 1;
        if (tokens[current + lookahead].type == TokenType::LET || tokens[current + lookahead].type == TokenType::MUT) {
            lookahead++;
        }

        if (tokens[current + lookahead].type == TokenType::IDENTIFIER &&
            tokens[current + lookahead + 1].type == TokenType::COLON) {
            return true;
        }

        return false;
    }


    std::unique_ptr<Expr> Parser::primary() {
        if (match({TokenType::FALSE})) return std::make_unique<LiteralExpr>("false");
        if (match({TokenType::TRUE})) return std::make_unique<LiteralExpr>("true");
        if (match({TokenType::NUMBER, TokenType::STRING})) {
            return std::make_unique<LiteralExpr>(previous().lexeme);
        }
        if (match({TokenType::IDENTIFIER})) {
            return std::make_unique<VariableExpr>(previous());
        }
        if (is_lambda()) {
             return lambda_expression();
        }
        if (match({TokenType::LEFT_PAREN})) {
            std::unique_ptr<Expr> expr = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_unique<GroupingExpr>(std::move(expr));
        }

        if (match({TokenType::LEFT_BRACKET})) {
            std::vector<std::unique_ptr<Expr>> elements;
            if (!check(TokenType::RIGHT_BRACKET)) {
                do {
                    elements.push_back(expression());
                } while (match({TokenType::COMMA}));
            }
            consume(TokenType::RIGHT_BRACKET, "Expect ']' after array elements.");
            return std::make_unique<ArrayExpr>(std::move(elements));
        }

        throw ParseError("Expect expression.");
    }

    std::unique_ptr<TypeExpr> Parser::type() {
        Token name = consume(TokenType::IDENTIFIER, "Expect type name.");
        return std::make_unique<TypeExpr>(name);
    }

    Token Parser::consume(TokenType type, const std::string& message) {
        if (check(type)) return advance();
        throw ParseError(message);
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

    void Parser::synchronize() {
        advance();

        while (!is_at_end()) {
            if (previous().type == TokenType::END) return; // Assuming END is a good sync point

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

    bool Parser::check(TokenType type) {
        if (is_at_end()) return false;
        return peek().type == type;
    }

    bool Parser::check_next(TokenType type) {
        if (is_at_end() || tokens[current + 1].type == TokenType::EOF_TOKEN) {
            return false;
        }
        return tokens[current + 1].type == type;
    }

    Token Parser::peek_next() {
        if (is_at_end() || tokens[current + 1].type == TokenType::EOF_TOKEN) {
            return tokens[current];
        }
        return tokens[current + 1];
    }

    Token Parser::advance() {
        if (!is_at_end()) current++;
        return previous();
    }

    Token Parser::peek() {
        return tokens[current];
    }

    Token Parser::previous() {
        return tokens[current - 1];
    }

    bool Parser::is_at_end() {
        return peek().type == TokenType::EOF_TOKEN;
    }

} // namespace nota
