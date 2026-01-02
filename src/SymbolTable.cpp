#include "SymbolTable.hpp"

SymbolTable::SymbolTable() {
    currentScope = std::make_unique<Scope>();
}

void SymbolTable::enterScope() {
    auto newScope = std::make_unique<Scope>();
    newScope->parent = std::move(currentScope);
    currentScope = std::move(newScope);
}

void SymbolTable::exitScope() {
    if (currentScope->parent) {
        currentScope = std::move(currentScope->parent);
    }
}

void SymbolTable::define(const std::string& name, Symbol symbol) {
    currentScope->symbols[name] = std::move(symbol);
}

Symbol* SymbolTable::resolve(const std::string& name) {
    Scope* scope = currentScope.get();
    while (scope) {
        if (scope->symbols.count(name)) {
            return &scope->symbols[name];
        }
        scope = scope->parent.get();
    }
    return nullptr;
}
