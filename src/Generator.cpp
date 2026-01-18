#include "Generator.hpp"
#include <stdexcept>

Generator::Generator() {}

GeneratedCode Generator::generate(const Program& program) {
    for (const auto& stmt : program.statements) {
        generateStatement(*stmt);
    }
    return {html_out.str(), css_out.str()};
}

void Generator::generateStatement(const Statement& stmt) {
    stmt.accept(*this);
}

#include <unordered_map>

// Helper to get the base class for a component
std::string getBaseClass(const std::string& type) {
    static const std::unordered_map<std::string, std::string> base_classes = {
        {"App", "nota-app"},
        {"Row", "nota-row"},
        {"Col", "nota-col"},
        {"Rect", "nota-rect"},
        {"Text", "nota-text"}
    };
    auto it = base_classes.find(type);
    if (it != base_classes.end()) {
        return it->second;
    }
    return ""; // Or some default
}

// Helper to get the HTML tag for a component
std::string getHtmlTag(const std::string& type) {
    if (type == "App") return "body";
    if (type == "Text") return "span";
    return "div";
}

void Generator::visit(const LiteralExpr& expr) {
    // Literals are handled by their parent statements
}

void Generator::visit(const IdentifierExpr& expr) {
    // Identifiers are handled by their parent statements
}

void Generator::visit(const PropertyStmt& stmt) {
    // This is handled inside visit(ComponentStmt)
}

void Generator::visit(const ComponentStmt& stmt) {
    if (stmt.type.lexeme == "App") {
        html_out << "<body class=\"nota-app\">";
        for (const auto& child : stmt.body) {
            generateStatement(*child);
        }
        html_out << "</body>";
        return;
    }

    std::string class_name = nextClassName();
    std::string base_class = getBaseClass(stmt.type.lexeme);
    std::string tag = getHtmlTag(stmt.type.lexeme);
    std::string text_content;

    html_out << "<" << tag << " class=\"" << base_class << " " << class_name << "\">";

    css_out << "." << class_name << " {\n";

    // Default styles based on component type
    if (base_class == "nota-row") {
        css_out << "    display: flex;\n";
        css_out << "    flex-direction: row;\n";
    } else if (base_class == "nota-col") {
        css_out << "    display: flex;\n";
        css_out << "    flex-direction: column;\n";
    } else {
        css_out << "    display: block;\n";
    }

    // Process properties
    for (const auto& child : stmt.body) {
        if (auto* prop = dynamic_cast<PropertyStmt*>(child.get())) {
            if (auto* value = dynamic_cast<LiteralExpr*>(prop->value.get())) {
                std::string prop_name = prop->name.lexeme;
                std::string prop_value = value->value.lexeme;

                // Handle string literal quotes
                if (value->value.type == TokenType::STRING) {
                    prop_value = prop_value.substr(1, prop_value.length() - 2);
                }

                if (prop_name == "width") {
                    css_out << "    width: " << prop_value << (isdigit(prop_value.back()) ? "px" : "") << ";\n";
                } else if (prop_name == "height") {
                    css_out << "    height: " << prop_value << (isdigit(prop_value.back()) ? "px" : "") << ";\n";
                } else if (prop_name == "color") {
                    css_out << "    background-color: " << prop_value << ";\n";
                } else if (prop_name == "spacing") {
                    css_out << "    gap: " << prop_value << "px;\n";
                } else if (prop_name == "text") {
                    text_content = prop_value;
                }
            }
        }
    }

    css_out << "}\n";

    // Process nested components after properties
    for (const auto& child : stmt.body) {
        if (dynamic_cast<ComponentStmt*>(child.get())) {
            generateStatement(*child);
        }
    }

    if (!text_content.empty()) {
        html_out << text_content;
    }

    html_out << "</" << tag << ">";
}

std::string Generator::nextClassName() {
    return "nota-class-" + std::to_string(class_counter++);
}
