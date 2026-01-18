#include "generator.h"
#include <iostream>

namespace nota {

std::string Generator::generate(const std::shared_ptr<Component>& root) {
    html.str("");
    css.str("");
    counter = 0;

    std::stringstream head;
    head << "<!DOCTYPE html>\n<html>\n<head>\n";
    head << "<style>\n";

    css << "* { box-sizing: border-box; overflow: hidden; margin: 0; padding: 0; }\n";

    // Generate body and collect css
    generateComponent(root);

    head << css.str();
    head << "</style>\n</head>\n";

    std::stringstream finalDoc;
    finalDoc << head.str() << html.str() << "</html>";

    return finalDoc.str();
}

void Generator::generateComponent(const std::shared_ptr<Component>& comp) {
    std::string className;
    std::string tag = "div";
    std::string textContent = "";

    if (comp->type == "App") {
        className = "nota-app";
        tag = "body";
    } else if (comp->type == "Text") {
        tag = "span";
        className = "nota-text-" + std::to_string(++counter);

        // Find text property
        for (const auto& prop : comp->properties) {
            if (prop->name == "text") {
                 // Assuming text value is a string literal
                 if (auto lit = std::dynamic_pointer_cast<Literal>(prop->value)) {
                     if (std::holds_alternative<std::string>(lit->value)) {
                         textContent = std::get<std::string>(lit->value);
                     }
                 }
            }
        }
    } else {
        className = "nota-" + comp->type + "-" + std::to_string(++counter);
    }

    css << "." << className << " {\n";
    css << generateStyles(comp, className);
    css << "}\n";

    html << "<" << tag << " class=\"" << className << "\"";

    // Check for id property
    for (const auto& prop : comp->properties) {
        if (prop->name == "id") {
            if (auto lit = std::dynamic_pointer_cast<Literal>(prop->value)) {
                 if (std::holds_alternative<std::string>(lit->value)) {
                     html << " id=\"" << std::get<std::string>(lit->value) << "\"";
                 }
            } else if (auto id = std::dynamic_pointer_cast<Identifier>(prop->value)) {
                 html << " id=\"" << id->name << "\"";
            }
        }
    }

    html << ">";

    if (!textContent.empty()) {
        html << textContent;
    }

    if (!comp->children.empty()) {
        html << "\n";
        for (const auto& child : comp->children) {
            generateComponent(child);
        }
    }

    html << "</" << tag << ">\n";
}

std::string Generator::generateStyles(const std::shared_ptr<Component>& comp, const std::string& className) {
    std::stringstream ss;

    // Default display
    if (comp->type == "Row") {
        ss << "display: flex;\nflex-direction: row;\n";
    } else if (comp->type == "Col") {
        ss << "display: flex;\nflex-direction: column;\n";
    } else {
        ss << "display: block;\n";
    }

    for (const auto& prop : comp->properties) {
        std::string val = getValue(prop->value);
        if (prop->name == "width") {
            ss << "width: " << val << ";\n";
        } else if (prop->name == "height") {
            ss << "height: " << val << ";\n";
        } else if (prop->name == "color") {
            if (comp->type == "Text") {
                ss << "color: " << val << ";\n";
            } else {
                ss << "background-color: " << val << ";\n";
            }
        } else if (prop->name == "spacing") {
            ss << "gap: " << val << ";\n";
        } else if (prop->name == "padding") {
            ss << "padding: " << val << ";\n";
        } else if (prop->name == "radius") {
            ss << "border-radius: " << val << ";\n";
        }
    }
    return ss.str();
}

std::string Generator::getValue(const std::shared_ptr<Expression>& expr) {
    if (auto lit = std::dynamic_pointer_cast<Literal>(expr)) {
        if (std::holds_alternative<int>(lit->value)) {
             return std::to_string(std::get<int>(lit->value)) + "px";
        } else if (std::holds_alternative<double>(lit->value)) {
             return std::to_string(std::get<double>(lit->value)) + "px";
        } else if (std::holds_alternative<std::string>(lit->value)) {
             return std::get<std::string>(lit->value);
        } else if (std::holds_alternative<bool>(lit->value)) {
             return std::get<bool>(lit->value) ? "true" : "false";
        }
    } else if (auto id = std::dynamic_pointer_cast<Identifier>(expr)) {
        return id->name;
    } else if (auto bin = std::dynamic_pointer_cast<BinaryExpression>(expr)) {
        return "calc(" + getValue(bin->left) + " " + bin->op + " " + getValue(bin->right) + ")";
    } else if (auto grp = std::dynamic_pointer_cast<GroupExpression>(expr)) {
        return "(" + getValue(grp->expression) + ")";
    } else if (auto mem = std::dynamic_pointer_cast<MemberExpression>(expr)) {
        // Just return simple representation for now, e.g. box.width
        // In real implementation this might map to var(--box-width)
        return getValue(mem->object) + "." + mem->member;
    }
    return "";
}

}
