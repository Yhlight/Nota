#include "SemanticAnalyzer.hpp"

namespace Nota {

class TypeChecker : public ExprVisitor {
public:
    TypeChecker(std::vector<std::string>& errors) : errors_(errors) {}

    std::any visit(const BinaryExpr& expr) override {
        // For now, we don't have complex type rules for binary ops
        return {};
    }

    std::any visit(const LiteralExpr& expr) override {
        // The type is inherent in the token, so we don't do much here.
        // This is where you'd check for things like `var x: int = "string"`
        return {};
    }

    std::any visit(const GroupingExpr& expr) override {
        return expr.expression->accept(*this);
    }

private:
    std::vector<std::string>& errors_;
};


SemanticAnalyzer::SemanticAnalyzer() : typeChecker_(*new TypeChecker(errors_)) {}

SemanticAnalyzer::~SemanticAnalyzer() {
    delete &typeChecker_;
}

void SemanticAnalyzer::Analyze(const ComponentNode& root) {
    root.accept(*this);
}

void SemanticAnalyzer::visit(const ComponentNode& node) {
    // Define the component itself in the symbol table if it has an 'id'
    for (const auto& prop : node.properties) {
        if (prop->name.lexeme == "id") {
            if (auto literal = dynamic_cast<LiteralExpr*>(prop->value.get())) {
                if (auto id = std::get_if<std::string>(&literal->value)) {
                    // Check if the id is already defined in any scope
                    if (symbolTable_.Resolve(*id)) {
                        errors_.push_back("Error: Component with id '" + *id + "' is already defined.");
                    } else {
                        symbolTable_.Define(*id, &node);
                    }
                }
            }
        }
    }

    // Each component defines a new scope for its children
    symbolTable_.EnterScope();

    // Visit all properties
    for (const auto& prop : node.properties) {
        prop->accept(*this);
    }

    // Visit all children
    for (const auto& child : node.children) {
        child->accept(*this);
    }

    symbolTable_.ExitScope();
}

void SemanticAnalyzer::visit(const PropertyNode& node) {
    CheckProperty(node);
    if (node.value) {
        node.value->accept(typeChecker_);
    }
}

void SemanticAnalyzer::CheckProperty(const PropertyNode& node) {
    const std::string& propName = node.name.lexeme;

    if (propName == "width" || propName == "height") {
        if (auto* literal = dynamic_cast<LiteralExpr*>(node.value.get())) {
            if (literal->type != TokenType::NUMBER && literal->type != TokenType::PERCENTAGE) {
                errors_.push_back("Error: Property '" + propName + "' expects a number or percentage.");
            }
        }
    } else if (propName == "color") {
        if (auto* literal = dynamic_cast<LiteralExpr*>(node.value.get())) {
            if (literal->type != TokenType::COLOR_HEX) {
                errors_.push_back("Error: Property 'color' expects a hex color value (e.g., #ffffff).");
            }
        }
    } else if (propName == "text") {
        if (auto* literal = dynamic_cast<LiteralExpr*>(node.value.get())) {
            if (literal->type != TokenType::STRING) {
                errors_.push_back("Error: Property 'text' expects a string.");
            }
        }
    }
}

} // namespace Nota
