#include "CodeGenerator.h"

#include <map>

// Helper map for component to tag mapping
static const std::map<std::string, std::string> component_tag_map = {
    {"App", "body"},
    {"Row", "div"},
    {"Col", "div"},
    {"Rect", "div"},
    {"Text", "span"},
};

CompilationResult CodeGenerator::generate(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        statement->accept(*this);
    }
    return {html_out.str(), css_out.str()};
}

std::any CodeGenerator::visit(const ComponentStmt& stmt) {
    std::string class_name = "nota-" + stmt.name.lexeme + "-" + std::to_string(class_counter++);
    std::string tag = component_tag_map.count(stmt.name.lexeme) ? component_tag_map.at(stmt.name.lexeme) : "div";

    html_out << "<" << tag << " class=\"" << class_name << "\">";

    css_out << "." << class_name << " {\n";
    // Default styles
    css_out << "  box-sizing: border-box;\n";
    css_out << "  overflow: hidden;\n";
    css_out << "  margin: 0;\n";
    css_out << "  padding: 0;\n";

    if (stmt.name.lexeme == "Row") {
        css_out << "  display: flex;\n";
        css_out << "  flex-direction: row;\n";
    } else if (stmt.name.lexeme == "Col") {
        css_out << "  display: flex;\n";
        css_out << "  flex-direction: column;\n";
    }

    // Process properties first
    for (const auto& child_stmt : stmt.body) {
        if (dynamic_cast<PropertyStmt*>(child_stmt.get())) {
            child_stmt->accept(*this);
        }
    }
    css_out << "}\n"; // Close the current component's CSS rule

    // Process nested components next
    for (const auto& child_stmt : stmt.body) {
        if (dynamic_cast<ComponentStmt*>(child_stmt.get())) {
            child_stmt->accept(*this);
        }
    }

    html_out << "</" << tag << ">";
    return {};
}

std::any CodeGenerator::visit(const PropertyStmt& stmt) {
    css_out << "  " << stmt.name.lexeme << ": ";
    std::any value = stmt.value->accept(*this);
    if (value.type() == typeid(double)) {
        css_out << std::any_cast<double>(value) << "px";
    } else if (value.type() == typeid(std::string)) {
        css_out << std::any_cast<std::string>(value);
    }
    css_out << ";\n";
    return {};
}

std::any CodeGenerator::visit(const LiteralExpr& expr) {
    return expr.value;
}

std::any CodeGenerator::visit(const IdentifierExpr& expr) {
    // To be implemented
    return {};
}

std::any CodeGenerator::visit(const BinaryExpr& expr) {
    // To be implemented
    return {};
}

std::any CodeGenerator::visit(const GetExpr& expr) {
    // To be implemented
    return {};
}
