#include "SymbolTable.h"

SymbolTable::SymbolTable(std::shared_ptr<SymbolTable> parent)
    : parent(std::move(parent)) {}

bool SymbolTable::insert(const std::string& name, std::shared_ptr<Symbol> symbol) {
    if (symbols.count(name)) {
        return false; // Symbol already exists
    }
    symbols[name] = std::move(symbol);
    return true;
}

std::shared_ptr<Symbol> SymbolTable::lookup(const std::string& name) const {
    if (symbols.count(name)) {
        return symbols.at(name);
    }
    if (parent) {
        return parent->lookup(name);
    }
    return nullptr;
}
