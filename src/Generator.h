#ifndef NOTA_COMPILER_GENERATOR_H
#define NOTA_COMPILER_GENERATOR_H

#include "AST.h"
#include <string>
#include <sstream>
#include <map>

class Generator {
public:
    // 接受 AST 根节点进行代码生成
    std::string Generate(const ProgramNode& program);

private:
    // 预处理阶段，扫描并注册所有组件定义
    void preprocessDefinitions(const ProgramNode& program);

    // 递归遍历 AST 节点的函数
    void visit(const std::shared_ptr<ASTNode>& node);
    void visitComponent(const ComponentNode& component);
    void visitProperty(const PropertyNode& property);

    // 辅助函数，用于生成 CSS 和 HTML
    std::string generateComponentTag(const ComponentNode& component);
    std::string generateCssClass(const ComponentNode& component);
    std::string getCssPropertyName(const std::string& notaName);
    std::string getCssValue(const PropertyValue& value);

    // 使用 stringstream 来构建输出
    std::stringstream htmlOutput;
    std::stringstream cssOutput;

    // 当前正在处理的组件的 CSS 类名
    std::string currentClassName;

    // 组件注册表
    std::map<std::string, std::shared_ptr<ComponentNode>> componentRegistry;

    // 辅助函数，用于深拷贝一个组件节点
    std::shared_ptr<ComponentNode> deepCopyComponent(const std::shared_ptr<ComponentNode>& original);

    // 追踪顶级 App 组件类型
    std::string appComponentType;
};

#endif //NOTA_COMPILER_GENERATOR_H
