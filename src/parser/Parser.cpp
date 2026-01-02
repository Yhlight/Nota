#include "Parser.h"
#include <stdexcept>

Parser::Parser(Lexer& lexer) : lexer_(lexer) {}

RootNode Parser::parse() {
    advance(); // Load the first token
    RootNode root;

    while (!check(TokenType::END_OF_FILE)) {
        if (check(TokenType::ITEM)) {
            root.item_definitions.push_back(parse_item_definition());
        } else if (check(TokenType::IDENTIFIER)) {
            if (root.root_component) {
                error("Multiple root components are not allowed.");
                synchronize();
            } else {
                root.root_component = parse_component();
            }
        } else {
            error("Expected a component or Item definition.");
            synchronize();
        }
    }

    return root;
}

const std::vector<CompilerError>& Parser::errors() const {
    return errors_;
}

void Parser::advance() {
    previous_ = current_;
    current_ = lexer_.next_token();
    while (current_.type == TokenType::UNEXPECTED) {
        error("Lexer error: " + std::string(current_.text));
        current_ = lexer_.next_token();
    }
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    error(message);
}

bool Parser::check(TokenType type) const {
    return current_.type == type;
}

template <typename Node>
void Parser::parse_component_body(Node& node) {
    consume(TokenType::LEFT_BRACE, "Expected '{' after component type.");

    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
        if (check(TokenType::IDENTIFIER) || check(TokenType::ITEM)) {
            if (lexer_.peek_next_significant_char() == ':') {
                node->properties.push_back(parse_property());
            } else if (lexer_.peek_next_significant_char() == '.' || lexer_.peek_next_significant_char() == '[') {
                if constexpr (std::is_same_v<Node, std::unique_ptr<ComponentNode>>) {
                    auto target = parse_expression();
                    node->assignments.push_back(parse_assignment(std::move(target)));
                } else {
                    error("Assignments are not allowed in Item definitions.");
                    synchronize();
                }
            }
            else {
                node->children.push_back(parse_component());
            }
        } else {
            error("Unexpected token in component body.");
            advance(); // Skip the token to avoid an infinite loop
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after component body.");
}

std::unique_ptr<ItemNode> Parser::parse_item_definition() {
    consume(TokenType::ITEM, "Expected 'Item' keyword.");

    auto item = std::make_unique<ItemNode>();
    item->name = current_;
    consume(TokenType::IDENTIFIER, "Expected an identifier for the Item name.");

    parse_component_body(item);
    return item;
}

std::unique_ptr<ComponentNode> Parser::parse_component() {
    if (!check(TokenType::IDENTIFIER) && !check(TokenType::ITEM)) {
        error("Expected a component type identifier or 'Item'.");
        return nullptr;
    }

    auto component = std::make_unique<ComponentNode>();
    component->type = current_;
    advance();

    parse_component_body(component);
    return component;
}

PropertyNode Parser::parse_property() {
    PropertyNode prop;
    prop.name = current_;
    advance();

    consume(TokenType::COLON, "Expected ':' after property name.");

    prop.value = parse_value();

    match(TokenType::SEMICOLON);

    return prop;
}

ASTValue Parser::parse_value() {
    if (match(TokenType::STRING)) {
        return LiteralNode{std::string(previous_.text), previous_};
    }
    if (match(TokenType::NUMBER)) {
        return LiteralNode{std::stod(std::string(previous_.text)), previous_};
    }
    if (check(TokenType::IDENTIFIER)) {
        return parse_expression();
    }

    error("Expected a value (string, number, or identifier).");
    return LiteralNode{std::string(""), previous_};
}

std::unique_ptr<Expression> Parser::parse_expression() {
    auto expr = parse_primary();

    while (match(TokenType::DOT) || match(TokenType::LEFT_BRACKET)) {
        if (previous_.type == TokenType::DOT) {
            Token member = current_;
            consume(TokenType::IDENTIFIER, "Expected property name after '.'.");
            auto new_expr = std::make_unique<Expression>();
            new_expr->variant = MemberAccessNode{std::move(expr), member};
            expr = std::move(new_expr);
        } else {
            auto index = parse_expression();
            consume(TokenType::RIGHT_BRACKET, "Expected ']' after index.");
            auto new_expr = std::make_unique<Expression>();
            new_expr->variant = IndexAccessNode{std::move(expr), std::move(index)};
            expr = std::move(new_expr);
        }
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parse_primary() {
    auto expr = std::make_unique<Expression>();
    if (match(TokenType::IDENTIFIER)) {
        expr->variant = LiteralNode{std::string(previous_.text), previous_};
    } else if (match(TokenType::NUMBER)) {
        expr->variant = LiteralNode{std::stod(std::string(previous_.text)), previous_};
    }
    else {
        error("Expected an identifier or number for an expression.");
        expr->variant = LiteralNode{std::string(""), previous_};
    }
    return expr;
}

AssignmentNode Parser::parse_assignment(std::unique_ptr<Expression> target) {
    consume(TokenType::EQUAL, "Expected '=' after target.");
    AssignmentNode assignment;
    assignment.target = std::move(target);
    assignment.value = parse_value();
    match(TokenType::SEMICOLON);
    return assignment;
}


void Parser::synchronize() {
    advance();
    while (!check(TokenType::END_OF_FILE)) {
        if (previous_.type == TokenType::SEMICOLON) return;
        switch (current_.type) {
            case TokenType::ITEM:
            case TokenType::IDENTIFIER:
                return;
            default:
                break;
        }
        advance();
    }
}

void Parser::error(const std::string& message) {
    if (had_error_) return;
    had_error_ = true;
    errors_.push_back({message, previous_.line, previous_.column});
}
