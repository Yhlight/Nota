#include "SymbolTable.hpp"

namespace Nota {

SymbolTable::SymbolTable() {
    // Start with a global scope
    EnterScope();
}

void SymbolTable::EnterScope() {
    scopes_.emplace_back();
}

void SymbolTable::ExitScope() {
    if (!scopes_.empty()) {
        scopes_.pop_back();
    }
}

bool SymbolTable::Define(const std::string& name, const AstNode* node) {
    if (scopes_.empty()) {
        return false; // Should not happen if constructor calls EnterScope
    }
    // Check if already defined in the current scope
    if (scopes_.back().count(name)) {
        return false; // Redefinition error
    }
    scopes_.back()[name] = {name, node};
    return true;
}

std::optional<Symbol> SymbolTable::Resolve(const std::string& name) const {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        const auto& scope = *it;
        auto symbolIt = scope.find(name);
        if (symbolIt != scope.end()) {
            return symbolIt->second;
        }
    }
    return std::nullopt; // Not found
}

} // namespace Nota
