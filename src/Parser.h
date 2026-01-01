#ifndef NOTA_COMPILER_PARSER_H
#define NOTA_COMPILER_PARSER_H

#include "Lexer.h"
#include "AST.h"
#include <vector>
#include <string>
#include <memory>

class Parser {
public:
    explicit Parser(Lexer& lexer);

    // 解析整个程序
    std::shared_ptr<ProgramNode> ParseProgram();

    // 获取解析过程中遇到的错误
    const std::vector<std::string>& Errors() const;

private:
    // 移动到下一个 token
    void nextToken();

    // 解析单个顶级组件
    std::shared_ptr<ComponentNode> parseComponent();

    // 解析一个组件定义
    std::shared_ptr<ComponentDefinitionNode> parseComponentDefinition();

    // 解析组件或属性列表（在 {} 内部）
    void parseComponentBody(ComponentNode& component);

    // 解析单个属性
    std::shared_ptr<PropertyNode> parseProperty();

    // 辅助函数，用于断言和错误处理
    bool expectPeek(TokenType t);
    void peekError(TokenType t);

    Lexer& lexer;
    Token currentToken;
    Token peekToken;

    std::vector<std::string> errors;
};

#endif //NOTA_COMPILER_PARSER_H
