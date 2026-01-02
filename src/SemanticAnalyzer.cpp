#include "SemanticAnalyzer.hpp"
#include <set>

SemanticAnalyzer::SemanticAnalyzer() {
    // Pre-define built-in components and their valid properties
}

void SemanticAnalyzer::analyze(const Component& root) {
    root.accept(*this);
}

const std::vector<std::string>& SemanticAnalyzer::getErrors() const {
    return errors;
}

void SemanticAnalyzer::visit(const Component& expr) {
    std::string parentComponent = currentComponent;
    currentComponent = expr.name.lexeme;

    symbolTable.enterScope();
    symbolTable.define(expr.name.lexeme, {expr.name.lexeme});

    for (const auto& prop : expr.properties) {
        prop->accept(*this);
    }

    for (const auto& child : expr.children) {
        child->accept(*this);
    }

    symbolTable.exitScope();
    currentComponent = parentComponent;
}

void SemanticAnalyzer::visit(const Property& expr) {
    static const std::set<std::string> spacingComponents = {"Row", "Col"};

    if (expr.name.lexeme == "spacing") {
        if (spacingComponents.find(currentComponent) == spacingComponents.end()) {
            errors.push_back("Property 'spacing' can only be used in Row or Col components.");
        }
    }

    expr.value->accept(*this);
}

void SemanticAnalyzer::visit(const Literal& expr) {
    // Can add checks for literal types here if needed
}
