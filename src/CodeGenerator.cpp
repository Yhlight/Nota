#include "CodeGenerator.hpp"
#include <set>

bool hasPositionedChildren(const Component& component) {
    for (const auto& child : component.children) {
        for (const auto& prop : child->properties) {
            if (prop->name.lexeme == "x" || prop->name.lexeme == "y" || prop->name.lexeme == "position") {
                return true;
            }
        }
    }
    return false;
}

void CodeGenerator::generate(const Component& root) {
    root.accept(*this);
}

std::string CodeGenerator::getHTML() const {
    return html.str();
}

std::string CodeGenerator::getCSS() const {
    return css.str();
}

#include <unordered_map>

void CodeGenerator::visit(const Component& expr) {
    std::string className = "nota-" + std::to_string(classCounter++);

    // Map Nota component types to HTML tags
    std::unordered_map<std::string, std::string> tagMap = {
        {"App", "body"}, {"Row", "div"}, {"Col", "div"}, {"Rect", "div"}, {"Text", "span"}
    };
    std::string tagName = tagMap.count(expr.name.lexeme) ? tagMap[expr.name.lexeme] : "div";

    html << "<" << tagName << " class=\"" << className << "\">";

    // Handle 'text' property for Text components
    std::string textContent;
    for (const auto& prop : expr.properties) {
        if (prop->name.lexeme == "text") {
            if (auto* lit = dynamic_cast<Literal*>(prop->value.get())) {
                textContent = lit->value.lexeme.substr(1, lit->value.lexeme.length() - 2);
            }
        }
    }

    // Generate CSS for this component
    css << "." << className << " {" << std::endl;
    if (hasPositionedChildren(expr)) {
        css << "    position: relative;" << std::endl;
    }

    bool isPositioned = false;
    for (const auto& prop : expr.properties) {
        if (prop->name.lexeme == "x" || prop->name.lexeme == "y" || prop->name.lexeme == "position") {
            isPositioned = true;
            break;
        }
    }
    if (isPositioned) {
        css << "    position: absolute;" << std::endl;
    }

    for (const auto& prop : expr.properties) {
        prop->accept(*this);
    }
    css << "}" << std::endl;

    // Add text content if any
    html << textContent;

    // Visit children
    for (const auto& child : expr.children) {
        child->accept(*this);
    }

    html << "</" << tagName << ">";
}

void CodeGenerator::visit(const Property& expr) {
    if (expr.name.lexeme == "text") return; // Handled in component visit

    // Map Nota property names to CSS property names
    std::unordered_map<std::string, std::string> propertyMap = {
        {"width", "width"}, {"height", "height"}, {"color", "background-color"},
        {"spacing", "gap"}, {"padding", "padding"}, {"radius", "border-radius"},
        {"x", "left"}, {"y", "top"}, {"index", "z-index"}
    };

    if (propertyMap.count(expr.name.lexeme)) {
        currentProperty = expr.name.lexeme;
        css << "    " << propertyMap[expr.name.lexeme] << ": ";
        expr.value->accept(*this);
        css << ";" << std::endl;
        currentProperty = "";
    }
}

void CodeGenerator::visit(const Literal& expr) {
    if (expr.value.type == TokenType::STRING && expr.value.lexeme.front() == '"') {
        // Strip the surrounding quotes for CSS output
        css << expr.value.lexeme.substr(1, expr.value.lexeme.length() - 2);
    } else {
        css << expr.value.lexeme;
    }

    // Add "px" for numeric literals without a unit
    if (expr.value.type == TokenType::NUMBER && currentProperty != "index" && expr.value.lexeme.find('%') == std::string::npos) {
        css << "px";
    }
}
