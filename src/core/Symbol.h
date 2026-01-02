#pragma once

#include <string>
#include <memory>
#include <utility>

enum class SymbolType {
    COMPONENT,
    PROPERTY,
};

struct Symbol {
    std::string name;
    SymbolType type;
    const void* node; // Use a non-owning raw pointer to the AST node.

    Symbol(std::string name, SymbolType type, const void* node)
        : name(std::move(name)), type(type), node(node) {}
};
