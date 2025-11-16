#include "parser.hpp"
#include <stdexcept>
#include <vector>

namespace nota {

    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& message) : std::runtime_error(message) {}
    };

    Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

    std::vector<std::unique_ptr<Stmt>> Parser::parse() {
        std::vector<std::unique_ptr<Stmt>> statements;
        while (!isAtEnd()) {
            auto decl = declaration();
            if (decl) {
                statements.push_back(std::move(decl));
            }
        }
        return statements;
    }

    std::unique_ptr<Stmt> Parser::declaration() {
        try {
            if (match({TokenType::LET})) return varDeclaration(false);
            if (match({TokenType::MUT})) return varDeclaration(true);
            return statement();
        } catch (ParseError& error) {
            synchronize();
            return nullptr;
        }
    }

    std::unique_ptr<Stmt> Parser::varDeclaration(bool isMutable) {
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
        std::unique_ptr<Expr> initializer = nullptr;
        if (match({TokenType::EQUAL})) {
            initializer = expression();
        }
        return std::make_unique<VarDeclStmt>(name, std::move(initializer), isMutable);
    }

    std::unique_ptr<Stmt> Parser::statement() {
        if (match({TokenType::IF})) return ifStatement();
        if (match({TokenType::LEFT_BRACE})) {
            std::vector<std::unique_ptr<Stmt>> statements;
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                statements.push_back(declaration());
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
            return std::make_unique<BlockStmt>(std::move(statements));
        }
        return expressionStatement();
    }

    std::unique_ptr<Stmt> Parser::ifStatement() {
        std::unique_ptr<Expr> condition = expression();
        consume(TokenType::END, "Expect 'end' after if condition.");

        std::unique_ptr<Stmt> thenBranch = statement();
        std::unique_ptr<Stmt> elseBranch = nullptr;
        if (match({TokenType::ELSE})) {
            elseBranch = statement();
        }

        return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
    }

    std::unique_ptr<Stmt> Parser::expressionStatement() {
        std::unique_ptr<Expr> expr = expression();
        return std::make_unique<ExpressionStmt>(std::move(expr));
    }

    std::unique_ptr<Expr> Parser::expression() {
        return assignment();
    }

    std::unique_ptr<Expr> Parser::assignment() {
        std::unique_ptr<Expr> expr = equality();
        if (match({TokenType::EQUAL})) {
            Token equals = previous();
            std::unique_ptr<Expr> value = assignment();
            if (auto varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
                Token name = varExpr->name;
                return std::make_unique<AssignExpr>(name, std::move(value));
            }
            throw ParseError("Invalid assignment target.");
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
        return primary();
    }

    std::unique_ptr<Expr> Parser::primary() {
        if (match({TokenType::FALSE})) return std::make_unique<LiteralExpr>(false);
        if (match({TokenType::TRUE})) return std::make_unique<LiteralExpr>(true);
        if (match({TokenType::NUMBER, TokenType::STRING})) {
             if (previous().type == TokenType::NUMBER) {
                return std::make_unique<LiteralExpr>(std::stod(previous().lexeme));
            }
            return std::make_unique<LiteralExpr>(previous().lexeme);
        }
        if (match({TokenType::IDENTIFIER})) {
            return std::make_unique<VariableExpr>(previous());
        }
        if (match({TokenType::LEFT_PAREN})) {
            std::unique_ptr<Expr> expr = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_unique<GroupingExpr>(std::move(expr));
        }
        throw ParseError("Expect expression.");
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

    Token Parser::consume(TokenType type, const std::string& message) {
        if (check(type)) return advance();
        throw ParseError(message);
    }

    bool Parser::check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    Token Parser::advance() {
        if (!isAtEnd()) current++;
        return previous();
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

    void Parser::synchronize() {
        advance();
        while (!isAtEnd()) {
            if (previous().type == TokenType::END) return; // Assuming END terminates statements
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
                default:
                    // Do nothing
                    ;
            }
            advance();
        }
    }

} // namespace nota
