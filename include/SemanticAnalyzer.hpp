#ifndef NOTA_SEMANTIC_ANALYZER_HPP
#define NOTA_SEMANTIC_ANALYZER_HPP

#include "AST.hpp"
#include "SymbolTable.hpp"
#include <vector>
#include <string>

namespace Nota {

class SemanticAnalyzer : public AstVisitor {
public:
    SemanticAnalyzer();
    ~SemanticAnalyzer();

    void Analyze(const ComponentNode& root);
    const std::vector<std::string>& Errors() const { return errors_; }

    void visit(const ComponentNode& node) override;
    void visit(const PropertyNode& node) override;

private:
    SymbolTable symbolTable_;
    std::vector<std::string> errors_;
    class ExprVisitor& typeChecker_;

    void CheckProperty(const PropertyNode& node);
};

} // namespace Nota

#endif // NOTA_SEMANTIC_ANALYZER_HPP
