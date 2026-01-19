#include "Parser.h"
#include "ComponentRegistry.h"
#include <iostream>
#include <stdexcept>

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

std::unique_ptr<Node> Parser::parse() {
    if (isAtEnd()) return nullptr;
    return parseComponent();
}

std::vector<std::unique_ptr<Node>> Parser::parseAll() {
    std::vector<std::unique_ptr<Node>> nodes;
    while (!isAtEnd()) {
        auto node = parseComponent();
        if (node) {
            // Check if it's a definition placeholder
            if (auto* comp = dynamic_cast<ComponentNode*>(node.get())) {
                if (comp->type != "__DEFINITION__") {
                    nodes.push_back(std::move(node));
                }
            } else {
                // IfNode or other non-definition nodes are added
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

std::unique_ptr<Node> Parser::parseComponent() {
    // Handle If statement
    if (match(TokenType::If)) {
        consume(TokenType::LParen, "Expect '(' after 'if'.");
        auto condition = parseExpression();
        consume(TokenType::RParen, "Expect ')' after condition.");

        auto ifNode = std::make_unique<IfNode>();
        ifNode->condition = std::move(condition);

        consume(TokenType::LBrace, "Expect '{' before then branch.");
        while (!isAtEnd() && peek().type != TokenType::RBrace) {
            auto child = parseComponent();
            if (child) {
                ifNode->thenBranch.push_back(std::move(child));
            } else {
                throw std::runtime_error("Unexpected token in if branch: " + peek().value);
            }
        }
        consume(TokenType::RBrace, "Expect '}' after then branch.");

        if (match(TokenType::Else)) {
            consume(TokenType::LBrace, "Expect '{' before else branch.");
            while (!isAtEnd() && peek().type != TokenType::RBrace) {
                auto child = parseComponent();
                if (child) {
                    ifNode->elseBranch.push_back(std::move(child));
                } else {
                    throw std::runtime_error("Unexpected token in else branch: " + peek().value);
                }
            }
            consume(TokenType::RBrace, "Expect '}' after else branch.");
        }

        return ifNode;
    }

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
                        Token propName = consume(TokenType::Identifier, "Expect property name.");
                        consume(TokenType::Colon, "Expect ':' after property name.");

                        auto value = parseExpression();
                        templateNode->properties.push_back(PropertyNode{propName.value, std::move(value)});
                        match(TokenType::Semicolon);

                     } else if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::LBrace) {
                        auto child = parseComponent();
                        if (child) templateNode->children.push_back(std::move(child));
                     } else {
                         auto child = parseComponent();
                         if (child) templateNode->children.push_back(std::move(child));
                     }
                 } else {
                     // Could be If statement inside definition
                     if (peek().type == TokenType::If) {
                         auto child = parseComponent();
                         if (child) templateNode->children.push_back(std::move(child));
                     } else {
                        throw std::runtime_error("Expect property or child component.");
                     }
                 }
             }

             consume(TokenType::RBrace, "Expect '}' after component body.");

             ComponentRegistry::instance().defineComponent(name, std::move(templateNode));

             auto placeholder = std::make_unique<ComponentNode>();
             placeholder->type = "__DEFINITION__";
             return placeholder;
        }

        std::unique_ptr<ComponentNode> compNode;

        if (ComponentRegistry::instance().hasComponent(identifier)) {
            // Retrieve template as generic Node
            auto templateNode = ComponentRegistry::instance().getComponent(identifier); // returns const ComponentNode*

            // Clone and cast back to ComponentNode since we know the template is one
            auto cloned = templateNode->clone();
            compNode.reset(dynamic_cast<ComponentNode*>(cloned.release()));
        } else {
            compNode = std::make_unique<ComponentNode>();
            compNode->type = identifier;
        }

        consume(TokenType::LBrace, "Expect '{' after component type.");

        while (!isAtEnd() && peek().type != TokenType::RBrace) {
             if (peek().type == TokenType::Property) {
                 // property <type> <name>: <value>
                 consume(TokenType::Property, "Expect 'property' keyword.");
                 Token typeToken = consume(TokenType::Identifier, "Expect type after 'property'."); // Assuming type is identifier for now (int, bool, etc)
                 // Or we might need check for keywords if int/bool become keywords? Currently identifiers.

                 Token nameToken = consume(TokenType::Identifier, "Expect property name.");

                 std::unique_ptr<Expr> defaultVal = nullptr;
                 if (match(TokenType::Colon)) {
                     defaultVal = parseExpression();
                 }

                 compNode->propertyDefs.push_back(PropertyDefNode{typeToken.value, nameToken.value, std::move(defaultVal)});

                 // If there's a default value, also set it as current property value if not overridden?
                 // Usually definition sets default.
                 // For now, let's just store definition.
                 // If we want it to be usable, we might also push to properties if it has default.
                 if (!compNode->propertyDefs.back().defaultValue) {
                      // No default
                 } else {
                      // Add to properties so it can be evaluated?
                      // But properties vector is for assigned values.
                      // CodeGen should look up defaults if not in properties.
                      // For simplicity in MVP, let's push to properties too if it has value.
                      compNode->properties.push_back(PropertyNode{nameToken.value, compNode->propertyDefs.back().defaultValue->clone()});
                 }

                 match(TokenType::Semicolon);

             } else if (peek().type == TokenType::Identifier) {
                 if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::Colon) {
                    Token propName = consume(TokenType::Identifier, "Expect property name.");
                    consume(TokenType::Colon, "Expect ':' after property name.");

                    auto value = parseExpression();

                    bool found = false;
                    for (auto& prop : compNode->properties) {
                        if (prop.name == propName.value) {
                            prop.value = value->clone();
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        compNode->properties.push_back(PropertyNode{propName.value, std::move(value)});
                    }

                    match(TokenType::Semicolon);

                 } else {
                    auto child = parseComponent();
                    if (child) compNode->children.push_back(std::move(child));
                 }
             } else if (peek().type == TokenType::If) {
                 auto child = parseComponent();
                 if (child) compNode->children.push_back(std::move(child));
             } else {
                 throw std::runtime_error("Expect property or child component.");
             }
        }

        consume(TokenType::RBrace, "Expect '}' after component body.");
        return compNode;
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

        while (peek().type == TokenType::Identifier || peek().type == TokenType::Number) {
             Token nextTok = peek();
             bool noSpace = false;
             if (token.type == TokenType::Number && nextTok.type == TokenType::Identifier) {
                 static const std::vector<std::string> units = {"px", "em", "rem", "vw", "vh", "%", "s", "ms", "deg", "rad"};
                 for (const auto& u : units) {
                     if (nextTok.value == u) {
                         noSpace = true;
                         break;
                     }
                 }
             }

             value += (noSpace ? "" : " ") + advance().value;
             token = previous();
        }
        return std::make_unique<LiteralExpr>(value, token.type);
    }

    if (match(TokenType::Identifier)) {
        Token name = previous();
        std::string value = name.value;

        while (peek().type == TokenType::Identifier || peek().type == TokenType::Number) {
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
        return expr;
    }

    throw std::runtime_error("Expect expression.");
}
