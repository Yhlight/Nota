#include "Parser.h"
#include "ComponentRegistry.h"
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

const Token& Parser::peek(int offset) const {
    if (current + offset >= tokens.size()) {
        if (!tokens.empty()) return tokens.back();
    }
    return tokens[current + offset];
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::match(TokenType type) {
    if (isAtEnd()) return false;
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (match(type)) {
        return previous();
    }
    std::cerr << "Error at " << peek().line << ":" << peek().column << " - " << message << std::endl;
    throw std::runtime_error(message);
}

std::unique_ptr<ComponentNode> Parser::parse() {
    if (isAtEnd()) return nullptr;
    return parseComponent();
}

std::vector<std::unique_ptr<ComponentNode>> Parser::parseAll() {
    std::vector<std::unique_ptr<ComponentNode>> nodes;
    while (!isAtEnd()) {
        auto node = parseComponent();
        if (node) {
            if (node->type != "__DEFINITION__") {
                nodes.push_back(std::move(node));
            }
        } else {
             if (!isAtEnd()) {
                 throw std::runtime_error("Unexpected token: " + peek().value);
             }
        }
    }
    return nodes;
}

std::unique_ptr<ComponentNode> Parser::parseComponent() {
    if (match(TokenType::Identifier)) {
        std::string identifier = previous().value;

        // Check for "Item Definition": Item Name { ... }
        if (identifier == "Item" && peek().type == TokenType::Identifier && peek(1).type == TokenType::LBrace) {
             Token nameToken = consume(TokenType::Identifier, "Expect name for component definition.");
             std::string name = nameToken.value;
             consume(TokenType::LBrace, "Expect '{' after component name.");

             auto templateNode = std::make_unique<ComponentNode>();
             templateNode->type = "Item";

             while (!isAtEnd() && peek().type != TokenType::RBrace) {
                 if (peek().type == TokenType::Identifier) {
                     // Check for property vs child
                     if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::Colon) {
                        Token name = consume(TokenType::Identifier, "Expect property name.");
                        consume(TokenType::Colon, "Expect ':' after property name.");

                        auto value = parseExpression();
                        templateNode->properties.push_back(PropertyNode{name.value, std::move(value)});
                        match(TokenType::Semicolon);

                     } else if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::LBrace) {
                        auto child = parseComponent();
                        if (child) templateNode->children.push_back(std::move(child));
                     } else {
                         auto child = parseComponent();
                         if (child) templateNode->children.push_back(std::move(child));
                     }
                 } else {
                     throw std::runtime_error("Expect property or child component.");
                 }
             }

             consume(TokenType::RBrace, "Expect '}' after component body.");

             ComponentRegistry::instance().defineComponent(name, std::move(templateNode));

             auto placeholder = std::make_unique<ComponentNode>();
             placeholder->type = "__DEFINITION__";
             return placeholder;
        }

        std::unique_ptr<ComponentNode> node;

        if (ComponentRegistry::instance().hasComponent(identifier)) {
            const ComponentNode* templateNode = ComponentRegistry::instance().getComponent(identifier);
            node = templateNode->clone();
        } else {
            node = std::make_unique<ComponentNode>();
            node->type = identifier;
        }

        consume(TokenType::LBrace, "Expect '{' after component type.");

        while (!isAtEnd() && peek().type != TokenType::RBrace) {
             if (peek().type == TokenType::Identifier) {
                 if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::Colon) {
                    Token name = consume(TokenType::Identifier, "Expect property name.");
                    consume(TokenType::Colon, "Expect ':' after property name.");

                    auto value = parseExpression();

                    bool found = false;
                    for (auto& prop : node->properties) {
                        if (prop.name == name.value) {
                            prop.value = value->clone();
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        node->properties.push_back(PropertyNode{name.value, std::move(value)});
                    }

                    match(TokenType::Semicolon);

                 } else {
                    auto child = parseComponent();
                    if (child) node->children.push_back(std::move(child));
                 }
             } else {
                 throw std::runtime_error("Expect property or child component.");
             }
        }

        consume(TokenType::RBrace, "Expect '}' after component body.");
        return node;
    }
    return nullptr;
}

// Expression Parsing

std::unique_ptr<Expr> Parser::parseExpression() {
    return parseEquality();
}

std::unique_ptr<Expr> Parser::parseEquality() {
    auto expr = parseComparison();

    while (match(TokenType::BangEqual) || match(TokenType::EqualEqual)) {
        Token op = previous();
        auto right = parseComparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseComparison() {
    auto expr = parseTerm();

    while (match(TokenType::Greater) || match(TokenType::GreaterEqual) ||
           match(TokenType::Less) || match(TokenType::LessEqual)) {
        Token op = previous();
        auto right = parseTerm();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseTerm() {
    auto expr = parseFactor();

    while (match(TokenType::Minus) || match(TokenType::Plus)) {
        Token op = previous();
        auto right = parseFactor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseFactor() {
    auto expr = parseUnary();

    while (match(TokenType::Slash) || match(TokenType::Star)) {
        Token op = previous();
        auto right = parseUnary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseUnary() {
    if (match(TokenType::Bang) || match(TokenType::Minus)) {
        Token op = previous();
        auto right = parseUnary();
        // UnaryExpr not defined yet?
        // For now, let's treat -5 as 0-5 or just literals.
        // Or implement UnaryExpr.
        // Let's implement UnaryExpr or fallback.
        // CodeGen doesn't support Unary yet.
        // Let's implement it properly.
        // Wait, I defined UnaryExpr in AST plan but maybe not in code?
        // Let's just consume it and treat as "Value" in Literal? No, that's bad.
        // I should have defined UnaryExpr.
        // Let's stick to what I have. Literal, Binary, Variable.
        // If I encounter -5, Lexer gives Minus, Number(5).
        // I need UnaryExpr.
        // For MVP logic, maybe I skip Unary for now or just parse it but throw/hack.
        // Actually -5 is common.
        // Let's assume for now negative numbers are handled by Lexer?
        // No, Lexer `readNumber` doesn't handle negative.
        // Let's hack it: -5 becomes 0 - 5.
        auto zero = std::make_unique<LiteralExpr>("0", TokenType::Number);
        return std::make_unique<BinaryExpr>(std::move(zero), op, std::move(right));
    }
    return parsePrimary();
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    if (match(TokenType::False)) return std::make_unique<LiteralExpr>("false", TokenType::False);
    if (match(TokenType::True)) return std::make_unique<LiteralExpr>("true", TokenType::True);

    if (match(TokenType::Number) || match(TokenType::String)) {
        Token token = previous();
        std::string value = token.value;
        // Merge subsequent identifiers/numbers for CSS values
        while (peek().type == TokenType::Identifier || peek().type == TokenType::Number) {
             // Heuristic: if current token is Number and next is Identifier (unit), don't add space?
             // Or if tokens are adjacent (col + len == next col).
             // Token stores line/column.
             // But Token doesn't store length. We can approximate length from value.

             Token nextTok = peek();
             // bool adjacent = (token.line == nextTok.line && token.column + token.value.length() == nextTok.column);
             // We need to track the 'last consumed token' to check adjacency with 'nextTok'.

             // Simplification: if previous was Number and next is Identifier starting with 'p' (px, pt, pc) or 'e' (em, ex) or 'r' (rem) or '%' etc?
             // CSS units: px, em, rem, %, vw, vh, s, ms, deg, rad...
             // Usually start with letter or %.
             // If we just parsed a Number, and next is Identifier, we should probably attach it without space if it looks like a unit.
             // But "1 solid" -> space. "1px" -> no space.

             bool noSpace = false;
             if (token.type == TokenType::Number && nextTok.type == TokenType::Identifier) {
                 // Check if it looks like a unit?
                 // Let's just assume Number+Identifier = Unit?
                 // "1px" -> yes. "1 solid" -> maybe? No, solid is not unit usually.
                 // But "1em", "100vh".
                 // If we merge "1 solid", it becomes "1solid", which is invalid. "1 solid" is valid shorthand.
                 // So we need to distinguish "unit" from "keyword".
                 // Common units: px, em, rem, %, vw, vh, cm, mm, in, pt, pc.
                 static const std::vector<std::string> units = {"px", "em", "rem", "vw", "vh", "%", "s", "ms", "deg", "rad"};
                 for (const auto& u : units) {
                     if (nextTok.value == u) {
                         noSpace = true;
                         break;
                     }
                 }
             }

             value += (noSpace ? "" : " ") + advance().value;
             // Update 'token' to be the one we just consumed for next iteration
             token = previous();
        }
        return std::make_unique<LiteralExpr>(value, token.type);
    }

    if (match(TokenType::Identifier)) {
        // Variable or Multi-token value (like "left top")
        // This is where "position: left top" hits.
        // Lexer gives Identifier(left), Identifier(top).
        // Parser calls parseExpression -> parsePrimary -> consumes "left".
        // It returns VariableExpr("left").
        // But what about "top"?
        // The loop in parseComponent checks `match(Semicolon)`.
        // If "top" follows, `parseExpression` didn't consume it.
        // We need a way to consume sequence of identifiers as a single string literal if valid?
        // OR, `left top` is implicitly a list or space-separated value?
        // Nota.md says "position: left top".
        // Maybe we parse it as: Variable(left) Variable(top)? No.
        // If `parseExpression` returns one Expr, and we have more tokens before semicolon...
        // We probably need to support Space separated values in `parseExpression` or `parseProperty`.

        // Let's handle the "multi-token value" logic inside parseExpression?
        // Or maybe parseExpression returns the first one, and we check if there are more?
        // Standard expressions don't usually concatenate with space unless it's function call arguments.
        // CSS values often use spaces.
        // Let's modify parseExpression or parsePrimary to handle this CSS-ism.
        // If we see Identifier, we look ahead. If next is Identifier/Number, we consume it and append?

        Token name = previous();
        std::string value = name.value;

        while (peek().type == TokenType::Identifier || peek().type == TokenType::Number) {
             // Be careful not to consume newlines implicitly via skipWhitespace in Lexer?
             // Tokens are already lexed.
             value += " " + advance().value;
        }

        if (peek().type == TokenType::Dot) {
             while (match(TokenType::Dot)) {
                 if (match(TokenType::Identifier)) {
                     value += "." + previous().value;
                 } else {
                     throw std::runtime_error("Expect property name after dot.");
                 }
             }
             return std::make_unique<VariableExpr>(Token{TokenType::Identifier, value, name.line, name.column});
        }

        if (value != name.value) {
             return std::make_unique<LiteralExpr>(value, TokenType::String);
        }

        return std::make_unique<VariableExpr>(name);
    }

    if (match(TokenType::LParen)) {
        auto expr = parseExpression();
        consume(TokenType::RParen, "Expect ')' after expression.");
        return expr; // GroupingExpr not strictly needed for logic unless we want to preserve structure
    }

    throw std::runtime_error("Expect expression.");
}
