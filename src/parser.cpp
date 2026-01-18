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

std::shared_ptr<Program> Parser::parse() {
    auto program = std::make_shared<Program>();
    while (current().type != TokenType::EndOfFile) {
        program->addComponent(parseComponent());
    }
    return program;
}

std::shared_ptr<Component> Parser::parseComponent() {
    std::string type;
    std::string name = "";

    if (current().type == TokenType::Item) {
        type = "Item";
        advance();
        if (current().type == TokenType::Identifier) {
            name = current().text;
            advance();
        }
    } else if (current().type == TokenType::Identifier) {
        type = current().text;
        advance();
    } else {
        throw std::runtime_error("Expected Component Type or Item keyword");
    }

    expect(TokenType::LBrace);

    auto comp = std::make_shared<Component>(type, name);

    while (current().type != TokenType::RBrace && current().type != TokenType::EndOfFile) {
        if (current().type == TokenType::Semicolon) {
            advance();
            continue;
        }

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
    return parseTerm();
}

std::shared_ptr<Expression> Parser::parseTerm() {
    auto left = parseFactor();

    while (current().type == TokenType::Plus || current().type == TokenType::Minus) {
        std::string op = current().text;
        advance();
        auto right = parseFactor();
        left = std::make_shared<BinaryExpression>(left, op, right);
    }
    return left;
}

std::shared_ptr<Expression> Parser::parseFactor() {
    auto left = parsePrimary();

    while (current().type == TokenType::Star || current().type == TokenType::Slash) {
        std::string op = current().text;
        advance();
        auto right = parsePrimary();
        left = std::make_shared<BinaryExpression>(left, op, right);
    }
    return left;
}

std::shared_ptr<Expression> Parser::parsePrimary() {
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

        std::shared_ptr<Expression> expr = std::make_shared<Identifier>(t.text);

        while (current().type == TokenType::Dot) {
            advance();
            Token member = current();
            if (member.type != TokenType::Identifier) {
                throw std::runtime_error("Expected identifier after dot");
            }
            advance();
            expr = std::make_shared<MemberExpression>(expr, member.text);
        }

        // For backwards compatibility/simplicity where colors are identifiers
        // We wrap simple identifiers in Literal if they are not part of a member chain?
        // Or handle in generator. AST should preserve structure.
        // But previously I returned Literal for identifiers.
        // Let's verify usage. Lexer tests: color: red. red is Identifier.
        // If I return Identifier node, Generator needs to handle Identifier node.
        // Previous code: return std::make_shared<Literal>(t.text);
        // I should probably check if it was just a single identifier and return Literal to avoid breaking Generator if it expects Literal for colors.
        // BUT, MemberExpression needs Identifier as object.
        // Let's update Generator to handle Identifier nodes and treat them as strings/values.
        return expr;

    } else if (t.type == TokenType::LParen) {
        auto expr = parseExpression();
        expect(TokenType::RParen);
        return std::make_shared<GroupExpression>(expr);
    } else if (t.type == TokenType::This) {
        return std::make_shared<ThisExpression>();
    } else if (t.type == TokenType::Parent) {
        return std::make_shared<ParentExpression>();
    }

    throw std::runtime_error("Unexpected token in expression: " + t.text);
}

}
