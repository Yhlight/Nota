#include "Parser.h"

#include <map>

// Operator precedence table
static std::map<TokenType, int> precedence = {
    {TokenType::Plus, 10},
    {TokenType::Minus, 10},
    {TokenType::Asterisk, 20},
    {TokenType::Slash, 20},
    {TokenType::Percent, 20},
    {TokenType::LessThan, 5},
};

static int get_precedence(TokenType type) {
    if (precedence.count(type)) {
        return precedence[type];
    }
    return -1;
}

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

// Helper methods
Token Parser::peek() const {
    if (is_at_end()) {
        // Return a sentinel token to avoid out-of-bounds access
        return {TokenType::Unknown, ""};
    }
    return tokens[position];
}

Token Parser::advance() {
    if (!is_at_end()) {
        return tokens[position++];
    }
    return {TokenType::Unknown, ""};
}

bool Parser::is_at_end() const {
    return position >= tokens.size();
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (peek().type == type) {
        return advance();
    }
    // For now, we'll just return an empty token on error.
    // A more robust implementation would throw an exception.
    return {TokenType::Unknown, ""};
}


// Parsing methods

std::unique_ptr<Stmt> Parser::parse_statement() {
    if (peek().type == TokenType::Let) {
        return parse_let_statement();
    }
    if (peek().type == TokenType::If) {
        return parse_if_statement();
    }
    if (peek().type == TokenType::Match) {
        return parse_match_statement();
    }
    // Handle other statement types here...
    return nullptr;
}

std::unique_ptr<Stmt> Parser::parse_let_statement() {
    advance(); // Consume 'let'

    Token name = consume(TokenType::Identifier, "Expected identifier.");
    if (name.type == TokenType::Unknown) return nullptr;

    if (consume(TokenType::Equal, "Expected '='.").type == TokenType::Unknown) return nullptr;

    std::unique_ptr<Expr> initializer = parse_expression();
    if (!initializer) {
        // Error handling: expected an expression
        return nullptr;
    }

    return std::make_unique<VarDeclStmt>(name, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::parse_if_statement(bool is_else_if) {
    advance(); // Consume 'if'

    std::unique_ptr<Expr> condition = parse_expression();
    if (!condition) {
        // Error handling: expected an expression
        return nullptr;
    }

    std::unique_ptr<Stmt> then_branch = parse_block_statement();
    if (!then_branch) {
        // Error handling: expected a block
        return nullptr;
    }

    std::unique_ptr<Stmt> else_branch = nullptr;
    if (peek().type == TokenType::Else) {
        advance(); // Consume 'else'
        if (peek().type == TokenType::If) {
            // This is an 'else if'
            else_branch = parse_if_statement(true);
        } else {
            // This is a simple 'else'
            else_branch = parse_block_statement();
        }
    }

    if (!is_else_if) {
        if (consume(TokenType::End, "Expected 'end' after if statement.").type == TokenType::Unknown) {
            return nullptr;
        }
    }


    return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch),
                                      std::move(else_branch));
}

std::unique_ptr<Stmt> Parser::parse_match_statement() {
    advance(); // Consume 'match'

    std::unique_ptr<Expr> condition = parse_expression();
    if (!condition) {
        // Error handling
        return nullptr;
    }

    std::vector<MatchCase> cases;
    while (peek().type != TokenType::End && !is_at_end()) {
        std::vector<std::unique_ptr<Expr>> values;
        do {
            values.push_back(parse_expression());
        } while (peek().type == TokenType::Comma && advance().type != TokenType::Unknown);

        if (consume(TokenType::Colon, "Expected ':' after match case values.").type == TokenType::Unknown) {
            return nullptr;
        }

        std::unique_ptr<Stmt> body = parse_block_statement();
        if (!body) {
            return nullptr;
        }

        cases.emplace_back(std::move(values), std::move(body));
    }

    if (consume(TokenType::End, "Expected 'end' after match statement.").type == TokenType::Unknown) {
        return nullptr;
    }

    return std::make_unique<MatchStmt>(std::move(condition), std::move(cases));
}


std::unique_ptr<Stmt> Parser::parse_block_statement() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (peek().type != TokenType::End && peek().type != TokenType::Else && !is_at_end()) {
        auto stmt = parse_statement();
        if (stmt) {
            statements.push_back(std::move(stmt));
        } else {
            // If we can't parse a statement, assume we're at the end of the block.
            break;
        }
    }

    return std::make_unique<BlockStmt>(std::move(statements));
}


std::unique_ptr<Expr> Parser::parse_primary_expression() {
    if (peek().type == TokenType::Number) {
        return std::make_unique<NumberExpr>(advance());
    }
    if (peek().type == TokenType::Identifier) {
        return std::make_unique<IdentifierExpr>(advance());
    }
    // Handle other primary expression types here...
    return nullptr;
}

std::unique_ptr<Expr> Parser::parse_expression(int min_precedence) {
    std::unique_ptr<Expr> left = parse_primary_expression();

    while (!is_at_end()) {
        int prec = get_precedence(peek().type);
        if (prec < min_precedence) {
            break;
        }

        Token op = advance();
        std::unique_ptr<Expr> right = parse_expression(prec + 1);
        if (!right) {
            // Error handling: expected an expression
            return nullptr;
        }

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
            // If we can't parse a statement, we're probably at the end
            // of the file or encountered an error. For now, just stop.
            break;
        }
    }
    return program;
}
