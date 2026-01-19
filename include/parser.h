#pragma once

#include "lexer.h"
#include "ast.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<ProgramNode> parse();

private:
    std::vector<Token> tokens;
    size_t pos;

    Token peek(int offset = 0) const;
    Token advance();
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);

    std::shared_ptr<ImportNode> parseImport();
    std::shared_ptr<ComponentNode> parseComponent();
    std::shared_ptr<PropertyNode> parseProperty();
    std::shared_ptr<StructDefinitionNode> parseStruct();

    // Logic
    std::shared_ptr<ComponentNode> parseIf();

    // Expression Parsing
    std::shared_ptr<ExpressionNode> parseExpression();
    std::shared_ptr<ExpressionNode> parseTerm();
    std::shared_ptr<ExpressionNode> parseFactor();
    std::shared_ptr<ExpressionNode> parsePrimary();
    std::shared_ptr<ExpressionNode> parseBlock();
    std::shared_ptr<ListNode> parseList();
};
