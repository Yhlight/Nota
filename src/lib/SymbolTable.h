#ifndef NOTA_SYMBOL_TABLE_H
#define NOTA_SYMBOL_TABLE_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace nota {

struct Local {
    Token name;
    int depth;
};

class SymbolTable {
public:
    SymbolTable();

    void beginScope();
    void endScope();
    void addLocal(Token name);
    int resolveLocal(const std::string& name);

    std::vector<Local> locals;
    int scopeDepth;
};

} // namespace nota

#endif // NOTA_SYMBOL_TABLE_H
