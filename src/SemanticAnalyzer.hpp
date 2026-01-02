#ifndef NOTA_SEMANTIC_ANALYZER_HPP
#define NOTA_SEMANTIC_ANALYZER_HPP

#include "AST.hpp"
#include "SymbolTable.hpp"
#include <vector>
#include <string>
#include <set>

class SemanticAnalyzer : public ExprVisitor {
public:
    SemanticAnalyzer();
    void analyze(const Component& root);

    const std::vector<std::string>& getErrors() const;

    void visit(const Component& expr) override;
    void visit(const Property& expr) override;
    void visit(const Literal& expr) override;

private:
    SymbolTable symbolTable;
    std::vector<std::string> errors;
    std::string currentComponent;
};

#endif // NOTA_SEMANTIC_ANALYZER_HPP
