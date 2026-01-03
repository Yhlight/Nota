#include "parser/Parser.h"
#include <stdexcept>
#include <sstream>
#include <unordered_set>

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
        if (check(TokenType::STATE)) {
            if constexpr (std::is_same_v<Node, std::unique_ptr<ComponentNode>>) {
                node->states.push_back(parse_state_declaration());
            } else {
                error("State declarations are not allowed in Item definitions.");
                synchronize();
            }
        } else if (check(TokenType::IDENTIFIER)) {
            std::string_view name(current_.text);
            if (name.starts_with("on") && lexer_.peek_next_significant_char() == ':') {
                if constexpr (std::is_same_v<Node, std::unique_ptr<ComponentNode>>) {
                    node->event_handlers.push_back(parse_event_handler());
                } else {
                    error("Event handlers are not allowed in Item definitions.");
                    (void)parse_event_handler();
                }
            } else if (lexer_.peek_next_significant_char() == '.' || lexer_.peek_next_significant_char() == '[') {
                if constexpr (std::is_same_v<Node, std::unique_ptr<ComponentNode>>) {
                    auto target = parse_expression();
                    node->assignments.push_back(parse_assignment(std::move(target)));
                } else {
                    error("Assignments are not allowed in Item definitions.");
                    synchronize();
                }
            } else if (lexer_.peek_next_significant_char() == ':') {
                node->properties.push_back(parse_property());
            } else {
                node->children.push_back(parse_component());
            }
        } else if (check(TokenType::ITEM)) {
            node->children.push_back(parse_component());
        } else {
            error("Unexpected token in component body.");
            advance();
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' after component body.");
}

StateNode Parser::parse_state_declaration() {
    consume(TokenType::STATE, "Expected 'state' keyword.");
    StateNode state;
    state.name = current_;
    consume(TokenType::IDENTIFIER, "Expected state variable name.");
    consume(TokenType::COLON, "Expected ':' after state variable name.");
    state.value = parse_value();
    match(TokenType::SEMICOLON);
    return state;
}

EventHandlerNode Parser::parse_event_handler() {
    EventHandlerNode handler;
    handler.name = current_;
    consume(TokenType::IDENTIFIER, "Expected event handler name.");
    consume(TokenType::COLON, "Expected ':' after event handler name.");
    lexer_.reposition(current_.text.data());
    handler.body = lexer_.read_raw_block('{');
    advance();
    return handler;
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

bool is_position_keyword(TokenType type) {
    return type == TokenType::LEFT || type == TokenType::RIGHT ||
           type == TokenType::TOP || type == TokenType::BOTTOM ||
           type == TokenType::CENTER;
}

ASTValue Parser::parse_value() {
    if (match(TokenType::STRING)) {
        return LiteralNode{std::string(previous_.text), previous_};
    }
    if (is_position_keyword(current_.type)) {
        PositionNode pos_node;
        pos_node.first = { current_ };
        advance();
        if (is_position_keyword(current_.type)) {
            pos_node.second = { current_ };
            advance();
        }
        return pos_node;
    }
    return parse_expression();
}

std::unique_ptr<Expression> Parser::parse_expression() {
    return parse_term();
}

std::unique_ptr<Expression> Parser::parse_term() {
    auto expr = parse_factor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = previous_;
        auto right = parse_factor();
        auto new_expr = std::make_unique<Expression>();
        new_expr->variant = BinaryOperationNode{std::move(expr), op, std::move(right)};
        new_expr->line = op.line;
        new_expr->column = op.column;
        expr = std::move(new_expr);
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parse_factor() {
    auto expr = parse_unary();
    while (match(TokenType::STAR) || match(TokenType::SLASH)) {
        Token op = previous_;
        auto right = parse_unary();
        auto new_expr = std::make_unique<Expression>();
        new_expr->variant = BinaryOperationNode{std::move(expr), op, std::move(right)};
        new_expr->line = op.line;
        new_expr->column = op.column;
        expr = std::move(new_expr);
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parse_unary() {
    if (match(TokenType::MINUS)) {
        Token op = previous_;
        auto right = parse_unary();
        auto new_expr = std::make_unique<Expression>();
        auto zero_literal = std::make_unique<Expression>();
        zero_literal->variant = LiteralNode{0.0, op};
        zero_literal->line = op.line;
        zero_literal->column = op.column;
        new_expr->variant = BinaryOperationNode{std::move(zero_literal), op, std::move(right)};
        new_expr->line = op.line;
        new_expr->column = op.column;
        return new_expr;
    }
    return parse_call();
}

std::unique_ptr<Expression> Parser::parse_call() {
    auto expr = parse_primary();
    while (match(TokenType::DOT) || match(TokenType::LEFT_BRACKET)) {
        Token op = previous_;
        if (op.type == TokenType::DOT) {
            Token member = current_;
            consume(TokenType::IDENTIFIER, "Expected property name after '.'.");
            auto new_expr = std::make_unique<Expression>();
            new_expr->variant = MemberAccessNode{std::move(expr), member};
            new_expr->line = op.line;
            new_expr->column = op.column;
            expr = std::move(new_expr);
        } else {
            auto index = parse_expression();
            consume(TokenType::RIGHT_BRACKET, "Expected ']' after index.");
            auto new_expr = std::make_unique<Expression>();
            IndexAccessNode access_node{std::move(expr), std::move(index), op.line, op.column};
            new_expr->variant = std::move(access_node);
            new_expr->line = op.line;
            new_expr->column = op.column;
            expr = std::move(new_expr);
        }
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parse_primary() {
    auto expr = std::make_unique<Expression>();
    if (match(TokenType::IDENTIFIER)) {
        expr->variant = LiteralNode{std::string(previous_.text), previous_};
        expr->line = previous_.line;
        expr->column = previous_.column;
    } else if (match(TokenType::NUMBER)) {
        expr->variant = LiteralNode{std::stod(std::string(previous_.text)), previous_};
        expr->line = previous_.line;
        expr->column = previous_.column;
    } else if (match(TokenType::LEFT_PAREN)) {
        expr = parse_expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression.");
    } else {
        error("Expected an expression.");
        expr->variant = LiteralNode{std::string(""), previous_};
        expr->line = previous_.line;
        expr->column = previous_.column;
    }
    return expr;
}

AssignmentNode Parser::parse_assignment(std::unique_ptr<Expression> target) {
    consume(TokenType::COLON, "Expected ':' after target.");
    AssignmentNode assignment;
    assignment.target = std::move(target);
    assignment.value = parse_value();
    assignment.line = previous_.line;
    assignment.column = previous_.column;
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