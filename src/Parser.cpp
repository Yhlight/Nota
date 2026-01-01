#include "Parser.h"
#include <string>
#include <stdexcept>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    // 初始化 currentToken 和 peekToken
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    currentToken = peekToken;
    peekToken = lexer.NextToken();
}

const std::vector<std::string>& Parser::Errors() const {
    return errors;
}

void Parser::peekError(TokenType t) {
    std::string error = "Syntax Error: Expected next token to be " + std::to_string((int)t) +
                        ", got " + std::to_string((int)peekToken.type) + " instead.";
    errors.push_back(error);
}

bool Parser::expectPeek(TokenType t) {
    if (peekToken.type == t) {
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

std::shared_ptr<ProgramNode> Parser::ParseProgram() {
    auto program = std::make_shared<ProgramNode>();
    while (currentToken.type != TokenType::END_OF_FILE) {
        auto component = parseComponent();
        if (component) {
            program->components.push_back(component);
        }
        nextToken(); //
    }
    return program;
}


void Parser::parseComponentBody(ComponentNode& component) {
    nextToken(); // 跳过 '{'

    while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
        // 判断是解析属性还是子组件
        if (currentToken.type == TokenType::IDENTIFIER && peekToken.type == TokenType::COLON) {
            // 是一个属性
            auto prop = parseProperty();
            if (prop) {
                component.children.push_back(prop);
            }
        } else if (currentToken.type == TokenType::IDENTIFIER && peekToken.type == TokenType::LBRACE) {
            // 是一个子组件
            auto childComponent = parseComponent();
            if (childComponent) {
                component.children.push_back(childComponent);
            }
        }
        // 如果有分号，可以选择性地跳过它
        if (currentToken.type == TokenType::SEMICOLON) {
            nextToken();
        }
    }
}


std::shared_ptr<ComponentNode> Parser::parseComponent() {
    if (currentToken.type != TokenType::IDENTIFIER) {
        return nullptr;
    }

    auto component = std::make_shared<ComponentNode>();
    component->type = currentToken.literal;

    if (!expectPeek(TokenType::LBRACE)) {
        return nullptr;
    }

    parseComponentBody(*component);

    // 循环结束后，currentToken 应该是 '}'
    if (currentToken.type != TokenType::RBRACE) {
         // 错误：缺少闭合的 '}'
        return nullptr;
    }

    return component;
}


std::shared_ptr<PropertyNode> Parser::parseProperty() {
    if (currentToken.type != TokenType::IDENTIFIER) {
        return nullptr;
    }

    auto prop = std::make_shared<PropertyNode>();
    prop->name = currentToken.literal;

    if (!expectPeek(TokenType::COLON)) {
        return nullptr;
    }

    nextToken(); // 移动到值 token

    // 根据值的类型进行处理
    if (currentToken.type == TokenType::INTEGER) {
        prop->value = std::stoi(currentToken.literal);
    } else if (currentToken.type == TokenType::STRING || currentToken.type == TokenType::PERCENTAGE) {
        prop->value = currentToken.literal;
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        // 处理类似 `color: white` 的情况
        prop->value = currentToken.literal;
    }
    else {
        // 错误：无效的属性值
        return nullptr;
    }

    nextToken();

    return prop;
}
