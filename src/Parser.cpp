#include "Parser.h"
#include <stdexcept>

namespace nota {

    Parser::Parser(Lexer& lexer)
        : lexer(lexer) {

        // Initialize the rules map
        rules = {
            {TokenType::LeftParen, {nullptr, nullptr, PREC_NONE}},
            {TokenType::RightParen, {nullptr, nullptr, PREC_NONE}},
            {TokenType::LeftBrace, {nullptr, nullptr, PREC_NONE}},
            {TokenType::RightBrace, {nullptr, nullptr, PREC_NONE}},
            {TokenType::Comma, {nullptr, nullptr, PREC_NONE}},
            {TokenType::Dot, {nullptr, nullptr, PREC_NONE}},
            {TokenType::Minus, {&Parser::unary, &Parser::binary, PREC_TERM}},
            {TokenType::Plus, {nullptr, &Parser::binary, PREC_TERM}},
            {TokenType::Slash, {nullptr, &Parser::binary, PREC_FACTOR}},
            {TokenType::Star, {nullptr, &Parser::binary, PREC_FACTOR}},
            {TokenType::Bang, {&Parser::unary, nullptr, PREC_NONE}},
            {TokenType::BangEqual, {nullptr, &Parser::binary, PREC_EQUALITY}},
            {TokenType::EqualEqual, {nullptr, &Parser::binary, PREC_EQUALITY}},
            {TokenType::Greater, {nullptr, &Parser::binary, PREC_COMPARISON}},
            {TokenType::GreaterEqual, {nullptr, &Parser::binary, PREC_COMPARISON}},
            {TokenType::Less, {nullptr, &Parser::binary, PREC_COMPARISON}},
            {TokenType::LessEqual, {nullptr, &Parser::binary, PREC_COMPARISON}},
            {TokenType::Identifier, {nullptr, nullptr, PREC_NONE}},
            {TokenType::String, {nullptr, nullptr, PREC_NONE}},
            {TokenType::Integer, {&Parser::number, nullptr, PREC_NONE}},
            {TokenType::Float, {&Parser::number, nullptr, PREC_NONE}},
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
            statements.push_back(statement());
        }
        return statements;
    }

    std::unique_ptr<ast::Stmt> Parser::statement() {
        if (match(TokenType::Let) || match(TokenType::Mut)) {
            return var_declaration();
        }
        // Other statements will go here

        // Skip tokens until the next statement
        while(current_token.type != TokenType::Newline && current_token.type != TokenType::Eof)
        {
            advance();
        }
        if(current_token.type == TokenType::Newline) advance();

        return nullptr;
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

        consume(TokenType::Newline, "Expect newline after variable declaration.");
        return std::make_unique<ast::VarDeclStmt>(name, type, std::move(initializer));
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

    std::unique_ptr<ast::Expr> Parser::number() {
        return std::make_unique<ast::LiteralExpr>(previous_token);
    }

    std::unique_ptr<ast::Expr> Parser::binary(std::unique_ptr<ast::Expr> left) {
        Token op = previous_token;
        ParseRule& rule = get_rule(op.type);
        std::unique_ptr<ast::Expr> right = parse_precedence((Precedence)(rule.precedence + 1));
        return std::make_unique<ast::BinaryExpr>(std::move(left), op, std::move(right));
    }

} // namespace nota
