#ifndef NOTA_SYMBOL_TABLE_HPP
#define NOTA_SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>
#include <memory>

struct Symbol {
    std::string name;
    // Potentially more info here, like type
};

class SymbolTable {
public:
    SymbolTable();

    void enterScope();
    void exitScope();

    void define(const std::string& name, Symbol symbol);
    Symbol* resolve(const std::string& name);

private:
    struct Scope {
        std::unordered_map<std::string, Symbol> symbols;
        std::unique_ptr<Scope> parent;
    };

    std::unique_ptr<Scope> currentScope;
};

#endif // NOTA_SYMBOL_TABLE_HPP
