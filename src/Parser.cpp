#include "Parser.h"
#include "ComponentRegistry.h"
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

const Token& Parser::peek(int offset) const {
    if (current + offset >= tokens.size()) {
        // Return EOF token if out of bounds
        // Assuming the last token is EOF, or we can construct one.
        // But tokens is passed by value, we can't easily reference a static EOF.
        // Let's assume the lexer always adds EOF at the end.
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

bool Parser::match(TokenType type) {
    if (isAtEnd()) return false;
    if (peek().type == type) {
        current++;
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
    // Legacy support: if only one component, parse it.
    // Ideally we should use parseAll.
    if (isAtEnd()) return nullptr;
    return parseComponent();
}

std::vector<std::unique_ptr<ComponentNode>> Parser::parseAll() {
    std::vector<std::unique_ptr<ComponentNode>> nodes;
    while (!isAtEnd()) {
        auto node = parseComponent();
        if (node) {
            // Filter out definition placeholders
            if (node->type != "__DEFINITION__") {
                nodes.push_back(std::move(node));
            }
        } else {
             // Should verify if we are really at end or if parseComponent failed silently
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

             // Recursively parse the body.
             // We consume the LBrace here to reuse parsing logic? No.
             // We can treat it as a component definition.
             // Let's create a temporary node for the definition.
             // But wait, "Item Box { ... }" means Box is an Item.
             // So we parse the body as if it's an Item.

             consume(TokenType::LBrace, "Expect '{' after component name.");

             // Create the template node.
             // The type of the template is "Item" generally, but can we define "Rect Box"?
             // Nota.md says "Item Box".
             auto templateNode = std::make_unique<ComponentNode>();
             templateNode->type = "Item"; // Base type

             // Parse body logic (properties and children)
             // This logic is duplicated below, should extract.

             while (!isAtEnd() && peek().type != TokenType::RBrace) {
                 if (peek().type == TokenType::Identifier) {
                     // Lookahead for Property vs Component
                     bool isProperty = false;
                     // Check for Colon
                     if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::Colon) {
                         isProperty = true;
                     }
                     // Or Check for Identifier Identifier (multi-token property, but first must be ident)
                     // Parser heuristic for property: Ident : Value

                     if (isProperty) {
                        Token name = consume(TokenType::Identifier, "Expect property name.");
                        consume(TokenType::Colon, "Expect ':' after property name.");

                        std::string value;
                        if (match(TokenType::String) || match(TokenType::Number) || match(TokenType::Identifier)) {
                            value = previous().value;
                        } else {
                            throw std::runtime_error("Expect property value.");
                        }

                        while (peek().type == TokenType::Identifier || peek().type == TokenType::Number) {
                            if (current + 1 < tokens.size() && (tokens[current+1].type == TokenType::Colon || tokens[current+1].type == TokenType::LBrace)) {
                                break;
                            }
                            value += " " + consume(peek().type, "Consume value part").value;
                        }
                        templateNode->properties.push_back(PropertyNode{name.value, value});
                        match(TokenType::Semicolon);

                     } else if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::LBrace) {
                        // Child component (Component { ... })
                        auto child = parseComponent();
                        if (child) templateNode->children.push_back(std::move(child));
                     } else {
                         // Fallback for cases where it's not a property but looks like a child
                         // e.g. "Rect { ... }" or "Box { ... }"
                         auto child = parseComponent();
                         if (child) templateNode->children.push_back(std::move(child));
                     }
                 } else {
                     throw std::runtime_error("Expect property or child component.");
                 }
             }

             consume(TokenType::RBrace, "Expect '}' after component body.");

             // Register the component
             ComponentRegistry::instance().defineComponent(name, std::move(templateNode));

             // Return placeholder node
             auto placeholder = std::make_unique<ComponentNode>();
             placeholder->type = "__DEFINITION__";
             return placeholder;
        }

        // Standard Component or Instantiation
        std::unique_ptr<ComponentNode> node;

        if (ComponentRegistry::instance().hasComponent(identifier)) {
            // Instantiation
            const ComponentNode* templateNode = ComponentRegistry::instance().getComponent(identifier);
            node = templateNode->clone();
            // The type remains "Item" (or whatever the template was), but we might want to track the original type?
            // Nota output shows custom components resolve to their base components.
            // If Box is Item, output is Item (div).
        } else {
            // Built-in or unknown
            node = std::make_unique<ComponentNode>();
            node->type = identifier;
        }

        consume(TokenType::LBrace, "Expect '{' after component type.");

        while (!isAtEnd() && peek().type != TokenType::RBrace) {
             if (peek().type == TokenType::Identifier) {
                 if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::Colon) {
                    // Property
                    Token name = consume(TokenType::Identifier, "Expect property name.");
                    consume(TokenType::Colon, "Expect ':' after property name.");

                    std::string value;
                    if (match(TokenType::String) || match(TokenType::Number) || match(TokenType::Identifier)) {
                        value = previous().value;
                    } else {
                        throw std::runtime_error("Expect property value.");
                    }

                    while (peek().type == TokenType::Identifier || peek().type == TokenType::Number) {
                        if (current + 1 < tokens.size() && (tokens[current+1].type == TokenType::Colon || tokens[current+1].type == TokenType::LBrace)) {
                            break;
                        }
                        value += " " + consume(peek().type, "Consume value part").value;
                    }

                    // Override or Add
                    bool found = false;
                    for (auto& prop : node->properties) {
                        if (prop.name == name.value) {
                            prop.value = value;
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        node->properties.push_back(PropertyNode{name.value, value});
                    }

                    match(TokenType::Semicolon);

                 } else {
                    // Child component
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
