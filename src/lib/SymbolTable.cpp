#include "SymbolTable.h"

namespace nota {

SymbolTable::SymbolTable() : scopeDepth(0) {}

void SymbolTable::beginScope() {
    scopeDepth++;
}

void SymbolTable::endScope() {
    scopeDepth--;
}

void SymbolTable::addLocal(Token name) {
    locals.push_back({name, scopeDepth});
}

int SymbolTable::resolveLocal(const std::string& name) {
    for (int i = locals.size() - 1; i >= 0; i--) {
        if (locals[i].name.lexeme == name) {
            return i;
        }
    }
    return -1;
}

} // namespace nota
