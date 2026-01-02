#ifndef NOTA_CODE_GENERATOR_HPP
#define NOTA_CODE_GENERATOR_HPP

#include "AST.hpp"
#include <string>
#include <sstream>

class CodeGenerator : public ExprVisitor {
public:
    void generate(const Component& root);

    std::string getHTML() const;
    std::string getCSS() const;

    void visit(const Component& expr) override;
    void visit(const Property& expr) override;
    void visit(const Literal& expr) override;

private:
    std::stringstream html;
    std::stringstream css;
    int classCounter = 0;
};

#endif // NOTA_CODE_GENERATOR_HPP
