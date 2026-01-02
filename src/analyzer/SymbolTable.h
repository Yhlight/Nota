#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "core/Symbol.h"

class SymbolTable {
public:
    SymbolTable(std::shared_ptr<SymbolTable> parent = nullptr);

    bool insert(const std::string& name, std::shared_ptr<Symbol> symbol);
    std::shared_ptr<Symbol> lookup(const std::string& name) const;

    std::shared_ptr<SymbolTable> getParent() const { return parent; }

private:
    std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols;
    std::shared_ptr<SymbolTable> parent;
};
