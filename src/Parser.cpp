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
            {TokenType::DoubleColon, {nullptr, &Parser::get, PREC_CALL}},
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
            {TokenType::None, {&Parser::literal, nullptr, PREC_NONE}},
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
        error_at_token(current_token, message);
        had_error = true;
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
            auto stmt = statement();
            if (had_error) {
                synchronize();
            } else if(stmt) {
                statements.push_back(std::move(stmt));
            }
        }

        if (!errors.empty()) {
            return {};
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
        if (match(TokenType::Import)) {
            return import_statement();
        }
        if (match(TokenType::Package)) {
            return package_declaration();
        }
        if (match(TokenType::Return)) {
            auto value = expression();
            if (had_error) return nullptr;
            consume(TokenType::Newline, "Expect newline after return value.");
            return std::make_unique<ast::ReturnStmt>(std::move(value));
        }

        return expression_statement();
    }

    std::unique_ptr<ast::Stmt> Parser::var_declaration() {
        consume(TokenType::Identifier, "Expect variable name.");
        Token name = previous_token;

        std::unique_ptr<ast::Type> type = nullptr;
        if (match(TokenType::Colon)) {
            type = parse_type();
            if (had_error) return nullptr;
        }

        std::unique_ptr<ast::Expr> initializer = nullptr;
        if (match(TokenType::Equal)) {
            initializer = expression();
            if (had_error) return nullptr;
        }

        return std::make_unique<ast::VarDeclStmt>(name, std::move(type), std::move(initializer));
    }

    std::unique_ptr<ast::Stmt> Parser::if_statement() {
        auto first_condition = expression();
        if (had_error) return nullptr;
        consume(TokenType::Newline, "Expect newline after if condition.");
        auto first_then = block();

        auto head = std::make_unique<ast::IfStmt>(std::move(first_condition), std::move(first_then), nullptr);
        ast::IfStmt* current_if = head.get();

        while (match(TokenType::Else)) {
            if (match(TokenType::If)) {
                // This is an 'else if'
                auto condition = expression();
                if (had_error) return nullptr;
                consume(TokenType::Newline, "Expect newline after if condition.");
                auto then_branch = block();
                auto new_if = std::make_unique<ast::IfStmt>(std::move(condition), std::move(then_branch), nullptr);
                current_if->else_branch = std::move(new_if);
                current_if = static_cast<ast::IfStmt*>(current_if->else_branch.get());
            } else {
                // This is a final 'else'
                consume(TokenType::Newline, "Expect newline after else.");
                current_if->else_branch = block();
                // After the final else, the chain is done, so we break to consume 'end'.
                break;
            }
        }

        consume(TokenType::End, "Expect 'end' after if statement.");
        if (current_token.type == TokenType::Newline) advance();

        return head;
    }

    std::unique_ptr<ast::Stmt> Parser::while_statement() {
        auto condition = expression();
        if (had_error) return nullptr;
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
        if (had_error) return nullptr;
        consume(TokenType::Newline, "Expect newline after do-while condition.");

        return std::make_unique<ast::DoWhileStmt>(std::move(body), std::move(condition));
    }

    std::unique_ptr<ast::Stmt> Parser::match_statement() {
        auto expression = this->expression();
        if (had_error) return nullptr;
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
                if (had_error) return nullptr;
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
                std::unique_ptr<ast::Type> param_type = nullptr;
                if (match(TokenType::Colon)) {
                    param_type = parse_type();
                    if (had_error) return nullptr;
                }
                params.push_back({param_name, std::move(param_type)});
            } while (match(TokenType::Comma));
        }
        consume(TokenType::RightParen, "Expect ')' after parameters.");

        std::unique_ptr<ast::Type> return_type = nullptr;
        if (match(TokenType::Colon)) {
            return_type = parse_type();
            if (had_error) return nullptr;
        }

        consume(TokenType::Newline, "Expect newline after function signature.");
        auto body = block();
        consume(TokenType::End, "Expect 'end' after function body.");
        if (current_token.type == TokenType::Newline) advance();

        return std::make_unique<ast::FuncDeclStmt>(name, std::move(params), std::move(body), std::move(return_type));
    }

    std::unique_ptr<ast::Stmt> Parser::class_declaration() {
        consume(TokenType::Identifier, "Expect class name.");
        Token name = previous_token;
        consume(TokenType::Newline, "Expect newline after class name.");

        std::vector<std::unique_ptr<ast::FuncDeclStmt>> methods;
        while(match(TokenType::Func)) {
            methods.push_back(std::unique_ptr<ast::FuncDeclStmt>(dynamic_cast<ast::FuncDeclStmt*>(func_declaration().release())));
            if (had_error) return nullptr;
        }

        consume(TokenType::End, "Expect 'end' after class body.");
        if (current_token.type == TokenType::Newline) advance();

        return std::make_unique<ast::ClassDeclStmt>(name, std::move(methods));
    }

    std::unique_ptr<ast::Stmt> Parser::import_statement() {
        auto path = expression();
        if (had_error) return nullptr;

        std::optional<Token> alias;
        if (match(TokenType::As)) {
            consume(TokenType::Identifier, "Expect alias name.");
            alias = previous_token;
        }

        consume(TokenType::Newline, "Expect newline after import statement.");
        return std::make_unique<ast::ImportStmt>(std::move(path), alias);
    }

    std::unique_ptr<ast::Stmt> Parser::package_declaration() {
        consume(TokenType::Identifier, "Expect package name.");
        Token name = previous_token;
        consume(TokenType::Newline, "Expect newline after package name.");
        return std::make_unique<ast::PackageDeclStmt>(name);
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
            if (had_error) return nullptr;
            consume(TokenType::Newline, "Expect newline after for-each container.");
            auto body = block();
            consume(TokenType::End, "Expect 'end' after for-each block.");
            if (current_token.type == TokenType::Newline) advance();
            return std::make_unique<ast::ForEachStmt>(variable, std::move(container), std::move(body));
        }

        std::unique_ptr<ast::Stmt> initializer = nullptr;
        if (is_let) {
            initializer = var_declaration();
            if (had_error) return nullptr;
        } else if (!match(TokenType::Semicolon)) {
            initializer = std::make_unique<ast::ExpressionStmt>(expression());
            if (had_error) return nullptr;
        }
        if (initializer) {
            consume(TokenType::Semicolon, "Expect ';' after loop initializer.");
        }

        std::unique_ptr<ast::Expr> condition = nullptr;
        if (!match(TokenType::Semicolon)) {
            condition = expression();
            if (had_error) return nullptr;
            consume(TokenType::Semicolon, "Expect ';' after loop condition.");
        }

        std::unique_ptr<ast::Expr> increment = nullptr;
        if (!match(TokenType::Newline)) {
            increment = expression();
            if (had_error) return nullptr;
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
            if (had_error) return nullptr;
            consume(TokenType::Newline, "Expect newline after variable declaration.");
            return decl;
        }
        auto expr = expression();
        if (had_error) return nullptr;
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
            if (had_error) return nullptr;
        }
        return std::make_unique<ast::BlockStmt>(std::move(statements));
    }

    std::unique_ptr<ast::Expr> Parser::expression() {
        return parse_precedence(PREC_ASSIGNMENT);
    }

    std::unique_ptr<ast::Type> Parser::parse_type() {
        consume(TokenType::Identifier, "Expect type name.");
        std::unique_ptr<ast::Type> type = std::make_unique<ast::BaseType>(previous_token);

        while(match(TokenType::LeftBracket)) {
            std::unique_ptr<ast::Expr> size = nullptr;
            if (current_token.type != TokenType::RightBracket) {
                size = expression();
                if (had_error) return nullptr;
            }
            consume(TokenType::RightBracket, "Expect ']' after array size.");
            type = std::make_unique<ast::ArrayType>(std::move(type), std::move(size));
        }

        return type;
    }

    std::unique_ptr<ast::Expr> Parser::parse_precedence(Precedence precedence) {
        advance();
        PrefixParseFn prefix_rule = get_rule(previous_token.type).prefix;
        if (prefix_rule == nullptr) {
            error_at_token(previous_token, "Expect expression.");
            had_error = true;
            return nullptr;
        }

        std::unique_ptr<ast::Expr> left_expr = (this->*prefix_rule)();
        if (had_error) return nullptr;

        while (precedence <= get_rule(current_token.type).precedence) {
            advance();
            InfixParseFn infix_rule = get_rule(previous_token.type).infix;
            left_expr = (this->*infix_rule)(std::move(left_expr));
            if (had_error) return nullptr;
        }

        return left_expr;
    }

    ParseRule& Parser::get_rule(TokenType type) {
        return rules[type];
    }

    std::unique_ptr<ast::Expr> Parser::unary() {
        Token op = previous_token;
        std::unique_ptr<ast::Expr> right = parse_precedence(PREC_UNARY);
        if (had_error) return nullptr;
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
        if (had_error) return nullptr;
        consume(TokenType::RightParen, "Expect ')' after expression.");
        return expr;
    }

    std::unique_ptr<ast::Expr> Parser::binary(std::unique_ptr<ast::Expr> left) {
        Token op = previous_token;
        ParseRule& rule = get_rule(op.type);
        std::unique_ptr<ast::Expr> right = parse_precedence((Precedence)(rule.precedence + 1));
        if (had_error) return nullptr;
        return std::make_unique<ast::BinaryExpr>(std::move(left), op, std::move(right));
    }

    std::unique_ptr<ast::Expr> Parser::assignment(std::unique_ptr<ast::Expr> left) {
        if (dynamic_cast<ast::VariableExpr*>(left.get()) || dynamic_cast<ast::SubscriptExpr*>(left.get()) || dynamic_cast<ast::GetExpr*>(left.get())) {
            auto value = expression();
            if (had_error) return nullptr;
            return std::make_unique<ast::AssignExpr>(std::move(left), std::move(value));
        } else if (dynamic_cast<ast::SetExpr*>(left.get())) {
            auto value = expression();
            if (had_error) return nullptr;
            return std::make_unique<ast::AssignExpr>(std::move(left), std::move(value));
        }

        error_at_token(previous_token, "Invalid assignment target.");
        had_error = true;
        return left;
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
                if (had_error) return nullptr;
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
                std::unique_ptr<ast::Type> param_type = nullptr;
                if (match(TokenType::Colon)) {
                    param_type = parse_type();
                    if (had_error) return nullptr;
                }
                params.push_back({param_name, std::move(param_type)});
            } while (match(TokenType::Comma));
        }
        consume(TokenType::RightParen, "Expect ')' after parameters.");
        consume(TokenType::FatArrow, "Expect '=>' after lambda parameters.");

        std::variant<std::unique_ptr<ast::Expr>, std::unique_ptr<ast::Stmt>> body;
        if (match(TokenType::Newline)) {
            body = block();
            if (had_error) return nullptr;
            consume(TokenType::End, "Expect 'end' after lambda body.");
        } else {
            body = expression();
            if (had_error) return nullptr;
        }

        return std::make_unique<ast::LambdaExpr>(std::move(params), std::move(body));
    }

    std::unique_ptr<ast::Expr> Parser::array_literal() {
        std::vector<std::unique_ptr<ast::Expr>> elements;
        if (current_token.type != TokenType::RightBracket) {
            do {
                elements.push_back(expression());
                if (had_error) return nullptr;
            } while (match(TokenType::Comma));
        }
        consume(TokenType::RightBracket, "Expect ']' after array elements.");
        return std::make_unique<ast::ArrayLiteralExpr>(std::move(elements));
    }

    std::unique_ptr<ast::Expr> Parser::subscript(std::unique_ptr<ast::Expr> left) {
        auto index = expression();
        if (had_error) return nullptr;
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
                if (had_error) return nullptr;
            } while (match(TokenType::Comma));
        }
        consume(TokenType::RightBrace, "Expect '}' after arguments.");
        return std::make_unique<ast::CallExpr>(std::move(left), std::move(arguments));
    }

    const std::vector<std::string>& Parser::get_errors() const {
        return errors;
    }

    bool Parser::has_errors() const {
        return !errors.empty();
    }

    void Parser::error_at_token(const Token& token, const std::string& message) {
        if (had_error) return;
        std::string error_message = "[line " + std::to_string(token.line) + ":" + std::to_string(token.column) + "] Error";
        if (token.type == TokenType::Eof) {
            error_message += " at end";
        } else {
            error_message += " at '" + token.lexeme + "'";
        }
        error_message += ": " + message;
        errors.push_back(error_message);
    }

    void Parser::synchronize() {
        had_error = false;

        while (current_token.type != TokenType::Eof) {
            if (previous_token.type == TokenType::Newline) {
                return;
            }

            switch (current_token.type) {
                case TokenType::Class:
                case TokenType::Func:
                case TokenType::Let:
                case TokenType::Mut:
                case TokenType::For:
                case TokenType::If:
                case TokenType::While:
                case TokenType::Match:
                case TokenType::Return:
                case TokenType::Import:
                case TokenType::Package:
                    return;
                default:
                    ;
            }
            advance();
        }
    }

} // namespace nota
