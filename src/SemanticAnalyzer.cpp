#include "SemanticAnalyzer.hpp"
#include <set>

void SemanticAnalyzer::defineComponentProperties() {
    PropertyTypeMap globalProps;
    globalProps["width"] = {TokenType::NUMBER, TokenType::STRING};
    globalProps["height"] = {TokenType::NUMBER, TokenType::STRING};
    globalProps["color"] = {TokenType::STRING};
    globalProps["class"] = {TokenType::STRING};
    globalProps["id"] = {TokenType::IDENTIFIER};
    globalProps["padding"] = {TokenType::NUMBER, TokenType::STRING};
    globalProps["border"] = {TokenType::STRING};
    globalProps["radius"] = {TokenType::NUMBER};
    globalProps["x"] = {TokenType::NUMBER};
    globalProps["y"] = {TokenType::NUMBER};
    globalProps["index"] = {TokenType::NUMBER};

    componentPropTypes["App"] = globalProps;
    componentPropTypes["Row"] = globalProps;
    componentPropTypes["Col"] = globalProps;
    componentPropTypes["Rect"] = globalProps;
    componentPropTypes["Text"] = globalProps;

    componentPropTypes["Row"]["spacing"] = {TokenType::NUMBER};
    componentPropTypes["Col"]["spacing"] = {TokenType::NUMBER};
    componentPropTypes["Text"]["text"] = {TokenType::STRING};
}

SemanticAnalyzer::SemanticAnalyzer() {
    defineComponentProperties();
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
    if (componentPropTypes.count(currentComponent)) {
        const auto& props = componentPropTypes.at(currentComponent);
        if (props.count(expr.name.lexeme)) {
            const auto& allowedTypes = props.at(expr.name.lexeme);
            if (auto* literal = dynamic_cast<Literal*>(expr.value.get())) {
                if (allowedTypes.find(literal->value.type) == allowedTypes.end()) {
                    errors.push_back("Invalid type for property '" + expr.name.lexeme + "' in component '" + currentComponent + "'.");
                }
            }
        } else {
            errors.push_back("Undefined property '" + expr.name.lexeme + "' for component '" + currentComponent + "'.");
        }
    }

    expr.value->accept(*this);
}

void SemanticAnalyzer::visit(const Literal& expr) {
    // Can add checks for literal types here if needed
}
