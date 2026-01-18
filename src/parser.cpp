#include "parser.h"

namespace nota {

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), pos(0) {}

Token Parser::current() {
    if (pos >= tokens.size()) return {TokenType::EndOfFile, "", -1, -1};
    return tokens[pos];
}

Token Parser::peek() {
    if (pos + 1 >= tokens.size()) return {TokenType::EndOfFile, "", -1, -1};
    return tokens[pos + 1];
}

void Parser::advance() {
    pos++;
}

void Parser::expect(TokenType type) {
    if (current().type != type) {
        throw std::runtime_error("Unexpected token: " + current().text + " expected type " + std::to_string((int)type));
    }
    advance();
}

std::shared_ptr<Component> Parser::parse() {
    return parseComponent();
}

std::shared_ptr<Component> Parser::parseComponent() {
    if (current().type != TokenType::Identifier) {
        throw std::runtime_error("Expected Component Type Identifier");
    }
    std::string type = current().text;
    advance();

    expect(TokenType::LBrace);

    auto comp = std::make_shared<Component>(type);

    while (current().type != TokenType::RBrace && current().type != TokenType::EndOfFile) {
        if (current().type == TokenType::Identifier) {
            if (peek().type == TokenType::Colon) {
                comp->addProperty(parseProperty());
            } else if (peek().type == TokenType::LBrace) {
                comp->addChild(parseComponent());
            } else {
                 throw std::runtime_error("Unexpected token after identifier in component body: " + peek().text);
            }
        } else {
             throw std::runtime_error("Unexpected token in component body: " + current().text);
        }
    }

    expect(TokenType::RBrace);
    return comp;
}

std::shared_ptr<Property> Parser::parseProperty() {
    std::string name = current().text;
    advance();
    expect(TokenType::Colon);
    auto value = parseExpression();
    return std::make_shared<Property>(name, value);
}

std::shared_ptr<Expression> Parser::parseExpression() {
    Token t = current();
    advance();

    if (t.type == TokenType::Number) {
        if (current().type == TokenType::Percent) {
             advance();
             return std::make_shared<Literal>(t.text + "%");
        }
        if (t.text.find('.') != std::string::npos) {
             return std::make_shared<Literal>(std::stod(t.text));
        } else {
             return std::make_shared<Literal>(std::stoi(t.text));
        }
    } else if (t.type == TokenType::String) {
        return std::make_shared<Literal>(t.text);
    } else if (t.type == TokenType::Identifier) {
        if (t.text == "true") return std::make_shared<Literal>(true);
        if (t.text == "false") return std::make_shared<Literal>(false);
        // Treat as identifier reference or color if valid, for now generic identifier
        // Or if it's like 'red' treat as string literal for color?
        // Nota spec says: color: red;
        // So 'red' is an identifier in token stream.
        return std::make_shared<Literal>(t.text); // Treat identifiers as string values for now (like colors)
    }

    throw std::runtime_error("Unexpected token in expression: " + t.text);
}

}
