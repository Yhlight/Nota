#ifndef NOTA_GENERATOR_HPP
#define NOTA_GENERATOR_HPP

#include "AST.hpp"
#include "ComponentRegistry.hpp"
#include <string>
#include <sstream>
#include <unordered_map>

struct GeneratedCode {
    std::string html;
    std::string css;
};

class Generator : public ASTVisitor {
public:
    Generator();
    GeneratedCode generate(const Program& program);

    void visit(const LiteralExpr& expr) override;
    void visit(const IdentifierExpr& expr) override;
    void visit(const PropertyStmt& stmt) override;
    void visit(const ComponentStmt& stmt) override;
    void visit(const ComponentDefStmt& stmt) override;

private:
    void generateStatement(const Statement& stmt);

    std::unordered_map<std::string, const LiteralExpr*> property_overrides;
    std::ostringstream html_out;
    std::ostringstream css_out;
    int class_counter = 0;

    // Helper to generate a unique class name
    std::string nextClassName();
};

#endif // NOTA_GENERATOR_HPP
