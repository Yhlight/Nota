#include "Parser.h"
#include <stdexcept>

namespace nota {

    Parser::Parser(Lexer& lexer)
        : lexer(lexer) {

        // Initialize the rules map
        rules = {
            {TokenType::LeftParen, {&Parser::grouping, &Parser::call, PREC_CALL}},
            {TokenType::RightParen, {nullptr, nullptr, PREC_NONE}},
            {TokenType::LeftBrace, {nullptr, &Parser::initializer_list, PREC_CALL}},
            {TokenType::RightBrace, {nullptr, nullptr, PREC_NONE}},
            {TokenType::LeftBracket, {&Parser::array_literal, &Parser::subscript, PREC_CALL}},
            {TokenType::RightBracket, {nullptr, nullptr, PREC_NONE}},
            {TokenType::Comma, {nullptr, nullptr, PREC_NONE}},
            {TokenType::Dot, {nullptr, &Parser::get, PREC_CALL}},
            {TokenType::Semicolon, {nullptr, nullptr, PREC_NONE}},
            {TokenType::Minus, {&Parser::unary, &Parser::binary, PREC_TERM}},
            {TokenType::Plus, {nullptr, &Parser::binary, PREC_TERM}},
            {TokenType::Slash, {nullptr, &Parser::binary, PREC_FACTOR}},
            {TokenType::Star, {nullptr, &Parser::binary, PREC_FACTOR}},
            {TokenType::Bang, {&Parser::unary, nullptr, PREC_NONE}},
            {TokenType::BangEqual, {nullptr, &Parser::binary, PREC_EQUALITY}},
            {TokenType::Equal, {nullptr, &Parser::assignment, PREC_ASSIGNMENT}},
            {TokenType::EqualEqual, {nullptr, &Parser::binary, PREC_EQUALITY}},
            {TokenType::Greater, {nullptr, &Parser::binary, PREC_COMPARISON}},
            {TokenType::GreaterEqual, {nullptr, &Parser::binary, PREC_COMPARISON}},
            {TokenType::Less, {nullptr, &Parser::binary, PREC_COMPARISON}},
            {TokenType::LessEqual, {nullptr, &Parser::binary, PREC_COMPARISON}},
            {TokenType::AmpersandAmpersand, {nullptr, &Parser::binary, PREC_AND}},
            {TokenType::PipePipe, {nullptr, &Parser::binary, PREC_OR}},
            {TokenType::PlusPlus, {nullptr, &Parser::postfix, PREC_POSTFIX}},
            {TokenType::MinusMinus, {nullptr, &Parser::postfix, PREC_POSTFIX}},
            {TokenType::Identifier, {&Parser::variable, nullptr, PREC_NONE}},
            {TokenType::String, {&Parser::literal, nullptr, PREC_NONE}},
            {TokenType::Integer, {&Parser::literal, nullptr, PREC_NONE}},
            {TokenType::Float, {&Parser::literal, nullptr, PREC_NONE}},
            {TokenType::True, {&Parser::literal, nullptr, PREC_NONE}},
            {TokenType::False, {&Parser::literal, nullptr, PREC_NONE}},
            {TokenType::This, {&Parser::this_, nullptr, PREC_NONE}},
            {TokenType::Eof, {nullptr, nullptr, PREC_NONE}},
        };

        advance();
    }

    void Parser::advance() {
        previous_token = current_token;
        current_token = lexer.next_token();
    }

    void Parser::consume(TokenType type, const std::string& message) {
        if (current_token.type == type) {
            advance();
            return;
        }
        throw std::runtime_error(message);
    }

    bool Parser::match(TokenType type) {
        if (current_token.type == type) {
            advance();
            return true;
        }
        return false;
    }

    std::vector<std::unique_ptr<ast::Stmt>> Parser::parse() {
        std::vector<std::unique_ptr<ast::Stmt>> statements;
        while (current_token.type != TokenType::Eof) {
            if (match(TokenType::Newline)) {
                continue;
            }
            statements.push_back(statement());
        }
        return statements;
    }

    std::unique_ptr<ast::Stmt> Parser::statement() {
        if (match(TokenType::If)) {
            return if_statement();
        }
        if (match(TokenType::While)) {
            return while_statement();
        }
        if (match(TokenType::Do)) {
            return do_while_statement();
        }
        if (match(TokenType::For)) {
            return for_statement();
        }
        if (match(TokenType::Match)) {
            return match_statement();
        }
        if (match(TokenType::Func)) {
            return func_declaration();
        }
        if (match(TokenType::Class)) {
            return class_declaration();
        }
        if (match(TokenType::Return)) {
            auto value = expression();
            consume(TokenType::Newline, "Expect newline after return value.");
            return std::make_unique<ast::ReturnStmt>(std::move(value));
        }

        return expression_statement();
    }

    std::unique_ptr<ast::Stmt> Parser::var_declaration() {
        consume(TokenType::Identifier, "Expect variable name.");
        Token name = previous_token;

        std::optional<Token> type;
        if (match(TokenType::Colon)) {
            consume(TokenType::Identifier, "Expect type name.");
            type = previous_token;
        }

        std::unique_ptr<ast::Expr> initializer = nullptr;
        if (match(TokenType::Equal)) {
            initializer = expression();
        }

        return std::make_unique<ast::VarDeclStmt>(name, type, std::move(initializer));
    }

    std::unique_ptr<ast::Stmt> Parser::if_statement() {
        auto condition = expression();
        consume(TokenType::Newline, "Expect newline after if condition.");
        auto then_branch = block();

        std::unique_ptr<ast::Stmt> else_branch = nullptr;
        if (match(TokenType::Else)) {
            if (match(TokenType::If)) {
                else_branch = if_statement();
            } else {
                consume(TokenType::Newline, "Expect newline after else.");
                else_branch = block();
                consume(TokenType::End, "Expect 'end' after else block.");
            }
        } else {
            consume(TokenType::End, "Expect 'end' after if block.");
        }

        if(current_token.type == TokenType::Newline) advance();

        return std::make_unique<ast::IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
    }

    std::unique_ptr<ast::Stmt> Parser::while_statement() {
        auto condition = expression();
        consume(TokenType::Newline, "Expect newline after while condition.");
        auto body = block();
        consume(TokenType::End, "Expect 'end' after while block.");

        if(current_token.type == TokenType::Newline) advance();

        return std::make_unique<ast::WhileStmt>(std::move(condition), std::move(body));
    }

    std::unique_ptr<ast::Stmt> Parser::do_while_statement() {
        consume(TokenType::Newline, "Expect newline after 'do'.");
        auto body = block();
        consume(TokenType::While, "Expect 'while' after do block.");
        auto condition = expression();
        consume(TokenType::Newline, "Expect newline after do-while condition.");

        return std::make_unique<ast::DoWhileStmt>(std::move(body), std::move(condition));
    }

    std::unique_ptr<ast::Stmt> Parser::match_statement() {
        auto expression = this->expression();
        consume(TokenType::Newline, "Expect newline after match expression.");

        std::vector<ast::MatchCase> cases;
        std::unique_ptr<ast::Stmt> else_branch = nullptr;

        while (current_token.type != TokenType::End && current_token.type != TokenType::Eof) {
            if (match(TokenType::Underscore)) {
                consume(TokenType::Colon, "Expect ':' after default case.");
                consume(TokenType::Newline, "Expect newline after default case.");
                else_branch = block();
                consume(TokenType::End, "Expect 'end' after default case block.");
                if (current_token.type == TokenType::Newline) advance();
                break;
            }

            std::vector<std::unique_ptr<ast::Expr>> values;
            do {
                values.push_back(this->expression());
            } while (match(TokenType::Comma));

            consume(TokenType::Colon, "Expect ':' after match case values.");
            consume(TokenType::Newline, "Expect newline after match case.");

            auto body = block();
            consume(TokenType::End, "Expect 'end' after match case block.");
            if (current_token.type == TokenType::Newline) advance();

            cases.emplace_back(std::move(values), std::move(body));
        }

        consume(TokenType::End, "Expect 'end' after match statement.");
        if (current_token.type == TokenType::Newline) advance();

        return std::make_unique<ast::MatchStmt>(std::move(expression), std::move(cases), std::move(else_branch));
    }

    std::unique_ptr<ast::Stmt> Parser::func_declaration() {
        consume(TokenType::Identifier, "Expect function name.");
        Token name = previous_token;

        consume(TokenType::LeftParen, "Expect '(' after function name.");
        std::vector<ast::Param> params;
        if (current_token.type != TokenType::RightParen) {
            do {
                consume(TokenType::Identifier, "Expect parameter name.");
                Token param_name = previous_token;
                std::optional<Token> param_type;
                if (match(TokenType::Colon)) {
                    consume(TokenType::Identifier, "Expect parameter type.");
                    param_type = previous_token;
                }
                params.push_back({param_name, param_type});
            } while (match(TokenType::Comma));
        }
        consume(TokenType::RightParen, "Expect ')' after parameters.");

        std::optional<Token> return_type;
        if (match(TokenType::Colon)) {
            consume(TokenType::Identifier, "Expect return type.");
            return_type = previous_token;
        }

        consume(TokenType::Newline, "Expect newline after function signature.");
        auto body = block();
        consume(TokenType::End, "Expect 'end' after function body.");
        if (current_token.type == TokenType::Newline) advance();

        return std::make_unique<ast::FuncDeclStmt>(name, std::move(params), std::move(body), return_type);
    }

    std::unique_ptr<ast::Stmt> Parser::class_declaration() {
        consume(TokenType::Identifier, "Expect class name.");
        Token name = previous_token;
        consume(TokenType::Newline, "Expect newline after class name.");

        std::vector<std::unique_ptr<ast::FuncDeclStmt>> methods;
        while(match(TokenType::Func)) {
            methods.push_back(std::unique_ptr<ast::FuncDeclStmt>(dynamic_cast<ast::FuncDeclStmt*>(func_declaration().release())));
        }

        consume(TokenType::End, "Expect 'end' after class body.");
        if (current_token.type == TokenType::Newline) advance();

        return std::make_unique<ast::ClassDeclStmt>(name, std::move(methods));
    }

    std::unique_ptr<ast::Stmt> Parser::for_statement() {
        bool is_let = match(TokenType::Let);
        if ((is_let || current_token.type == TokenType::Identifier) && lexer.peek() == ':') {
            if (!is_let) {
                consume(TokenType::Identifier, "Expect variable name.");
            }
            Token variable = previous_token;
            consume(TokenType::Colon, "Expect ':' after variable name.");
            auto container = expression();
            consume(TokenType::Newline, "Expect newline after for-each container.");
            auto body = block();
            consume(TokenType::End, "Expect 'end' after for-each block.");
            if (current_token.type == TokenType::Newline) advance();
            return std::make_unique<ast::ForEachStmt>(variable, std::move(container), std::move(body));
        }

        std::unique_ptr<ast::Stmt> initializer = nullptr;
        if (is_let) {
            initializer = var_declaration();
        } else if (!match(TokenType::Semicolon)) {
            initializer = std::make_unique<ast::ExpressionStmt>(expression());
        }
        if (initializer) {
            consume(TokenType::Semicolon, "Expect ';' after loop initializer.");
        }

        std::unique_ptr<ast::Expr> condition = nullptr;
        if (!match(TokenType::Semicolon)) {
            condition = expression();
            consume(TokenType::Semicolon, "Expect ';' after loop condition.");
        }

        std::unique_ptr<ast::Expr> increment = nullptr;
        if (!match(TokenType::Newline)) {
            increment = expression();
        }
        consume(TokenType::Newline, "Expect newline after for clauses.");

        auto body = block();
        consume(TokenType::End, "Expect 'end' after for block.");
        if (current_token.type == TokenType::Newline) advance();

        return std::make_unique<ast::ForStmt>(std::move(initializer), std::move(condition), std::move(increment), std::move(body));
    }

    std::unique_ptr<ast::Stmt> Parser::expression_statement() {
        if (match(TokenType::Let) || match(TokenType::Mut)) {
            auto decl = var_declaration();
            consume(TokenType::Newline, "Expect newline after variable declaration.");
            return decl;
        }
        auto expr = expression();
        consume(TokenType::Newline, "Expect newline after expression.");
        return std::make_unique<ast::ExpressionStmt>(std::move(expr));
    }

    std::unique_ptr<ast::Stmt> Parser::block() {
        std::vector<std::unique_ptr<ast::Stmt>> statements;
        while (current_token.type != TokenType::End && current_token.type != TokenType::Else && current_token.type != TokenType::While && current_token.type != TokenType::Eof) {
            if (match(TokenType::Newline)) {
                continue;
            }
            statements.push_back(statement());
        }
        return std::make_unique<ast::BlockStmt>(std::move(statements));
    }

    std::unique_ptr<ast::Expr> Parser::expression() {
        return parse_precedence(PREC_ASSIGNMENT);
    }

    std::unique_ptr<ast::Expr> Parser::parse_precedence(Precedence precedence) {
        advance();
        PrefixParseFn prefix_rule = get_rule(previous_token.type).prefix;
        if (prefix_rule == nullptr) {
            throw std::runtime_error("Expect expression.");
        }

        std::unique_ptr<ast::Expr> left_expr = (this->*prefix_rule)();

        while (precedence <= get_rule(current_token.type).precedence) {
            advance();
            InfixParseFn infix_rule = get_rule(previous_token.type).infix;
            left_expr = (this->*infix_rule)(std::move(left_expr));
        }

        return left_expr;
    }

    ParseRule& Parser::get_rule(TokenType type) {
        return rules[type];
    }

    std::unique_ptr<ast::Expr> Parser::unary() {
        Token op = previous_token;
        std::unique_ptr<ast::Expr> right = parse_precedence(PREC_UNARY);
        return std::make_unique<ast::UnaryExpr>(op, std::move(right));
    }

    std::unique_ptr<ast::Expr> Parser::literal() {
        return std::make_unique<ast::LiteralExpr>(previous_token);
    }

    std::unique_ptr<ast::Expr> Parser::variable() {
        return std::make_unique<ast::VariableExpr>(previous_token);
    }

    std::unique_ptr<ast::Expr> Parser::grouping() {
        if (current_token.type == TokenType::RightParen ||
            (current_token.type == TokenType::Identifier && (lexer.peek() == ':' || lexer.peek() == ',' || lexer.peek() == ')'))) {
            return lambda_expression();
        }
        auto expr = expression();
        consume(TokenType::RightParen, "Expect ')' after expression.");
        return expr;
    }

    std::unique_ptr<ast::Expr> Parser::binary(std::unique_ptr<ast::Expr> left) {
        Token op = previous_token;
        ParseRule& rule = get_rule(op.type);
        std::unique_ptr<ast::Expr> right = parse_precedence((Precedence)(rule.precedence + 1));
        return std::make_unique<ast::BinaryExpr>(std::move(left), op, std::move(right));
    }

    std::unique_ptr<ast::Expr> Parser::assignment(std::unique_ptr<ast::Expr> left) {
        if (dynamic_cast<ast::VariableExpr*>(left.get()) || dynamic_cast<ast::SubscriptExpr*>(left.get()) || dynamic_cast<ast::GetExpr*>(left.get())) {
            auto value = expression();
            return std::make_unique<ast::AssignExpr>(std::move(left), std::move(value));
        }

        throw std::runtime_error("Invalid assignment target.");
    }

    std::unique_ptr<ast::Expr> Parser::postfix(std::unique_ptr<ast::Expr> left) {
        Token op = previous_token;
        return std::make_unique<ast::PostfixExpr>(std::move(left), op);
    }

    std::unique_ptr<ast::Expr> Parser::call(std::unique_ptr<ast::Expr> left) {
        std::vector<std::unique_ptr<ast::Expr>> arguments;
        if (current_token.type != TokenType::RightParen) {
            do {
                arguments.push_back(expression());
            } while (match(TokenType::Comma));
        }
        consume(TokenType::RightParen, "Expect ')' after arguments.");
        return std::make_unique<ast::CallExpr>(std::move(left), std::move(arguments));
    }

    std::unique_ptr<ast::Expr> Parser::lambda_expression() {
        std::vector<ast::Param> params;
        if (current_token.type != TokenType::RightParen) {
            do {
                consume(TokenType::Identifier, "Expect parameter name.");
                Token param_name = previous_token;
                std::optional<Token> param_type;
                if (match(TokenType::Colon)) {
                    consume(TokenType::Identifier, "Expect parameter type.");
                    param_type = previous_token;
                }
                params.push_back({param_name, param_type});
            } while (match(TokenType::Comma));
        }
        consume(TokenType::RightParen, "Expect ')' after parameters.");
        consume(TokenType::FatArrow, "Expect '=>' after lambda parameters.");

        std::variant<std::unique_ptr<ast::Expr>, std::unique_ptr<ast::Stmt>> body;
        if (match(TokenType::Newline)) {
            body = block();
            consume(TokenType::End, "Expect 'end' after lambda body.");
        } else {
            body = expression();
        }

        return std::make_unique<ast::LambdaExpr>(std::move(params), std::move(body));
    }

    std::unique_ptr<ast::Expr> Parser::array_literal() {
        std::vector<std::unique_ptr<ast::Expr>> elements;
        if (current_token.type != TokenType::RightBracket) {
            do {
                elements.push_back(expression());
            } while (match(TokenType::Comma));
        }
        consume(TokenType::RightBracket, "Expect ']' after array elements.");
        return std::make_unique<ast::ArrayLiteralExpr>(std::move(elements));
    }

    std::unique_ptr<ast::Expr> Parser::subscript(std::unique_ptr<ast::Expr> left) {
        auto index = expression();
        consume(TokenType::RightBracket, "Expect ']' after subscript index.");
        return std::make_unique<ast::SubscriptExpr>(std::move(left), std::move(index));
    }

    std::unique_ptr<ast::Expr> Parser::get(std::unique_ptr<ast::Expr> left) {
        consume(TokenType::Identifier, "Expect property name after '.'.");
        Token name = previous_token;
        return std::make_unique<ast::GetExpr>(std::move(left), name);
    }

    std::unique_ptr<ast::Expr> Parser::this_() {
        return std::make_unique<ast::ThisExpr>(previous_token);
    }

    std::unique_ptr<ast::Expr> Parser::initializer_list(std::unique_ptr<ast::Expr> left) {
        std::vector<std::unique_ptr<ast::Expr>> arguments;
        if (current_token.type != TokenType::RightBrace) {
            do {
                arguments.push_back(expression());
            } while (match(TokenType::Comma));
        }
        consume(TokenType::RightBrace, "Expect '}' after arguments.");
        return std::make_unique<ast::CallExpr>(std::move(left), std::move(arguments));
    }

} // namespace nota
