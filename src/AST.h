#ifndef NOTA_COMPILER_AST_H
#define NOTA_COMPILER_AST_H

#include <string>
#include <vector>
#include <memory>
#include <variant>

// --- 前置声明 ---
struct ComponentNode;
struct PropertyNode;
struct ComponentDefinitionNode;

// --- AST 节点基类 (未来可用于扩展) ---
struct ASTNode {
    virtual ~ASTNode() = default;
};

// --- 属性值的类型 ---
// 使用 variant 来存储不同类型的属性值
using PropertyValue = std::variant<int, std::string>;

// --- 属性节点 ---
// 例如: width: 100%
struct PropertyNode : public ASTNode {
    std::string name;
    PropertyValue value;
};

// --- 组件节点 ---
// 例如: App { ... }
struct ComponentNode : public ASTNode {
    std::string type; // 例如 "App", "Row", "Rect"
    std::vector<std::shared_ptr<ASTNode>> children; // 子节点，可以是组件或属性
};

// --- 组件定义节点 ---
// 例如: Item Box { Rect { ... } }
struct ComponentDefinitionNode : public ASTNode {
    std::string name; // "Box"
    std::shared_ptr<ComponentNode> body;
};

// --- 程序根节点 ---
// 代表一个完整的 .nota 文件
struct ProgramNode : public ASTNode {
    // 根节点可以包含组件定义和顶级的组件实例
    std::vector<std::shared_ptr<ASTNode>> children;
};


#endif //NOTA_COMPILER_AST_H
