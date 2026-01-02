#ifndef NOTA_SYMBOL_TABLE_HPP
#define NOTA_SYMBOL_TABLE_HPP

#include "AST.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <optional>

namespace Nota {

// Represents a symbol in the table
struct Symbol {
    std::string name;
    // We can store more info here, e.g., type, definition node
    const AstNode* definitionNode;
};

class SymbolTable {
public:
    SymbolTable();

    // Enters a new scope
    void EnterScope();

    // Exits the current scope
    void ExitScope();

    // Defines a new symbol in the current scope
    bool Define(const std::string& name, const AstNode* node);

    // Looks up a symbol in the current and enclosing scopes
    std::optional<Symbol> Resolve(const std::string& name) const;

private:
    using Scope = std::unordered_map<std::string, Symbol>;
    std::vector<Scope> scopes_;
};

} // namespace Nota

#endif // NOTA_SYMBOL_TABLE_HPP
