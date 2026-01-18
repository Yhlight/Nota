#include "generator.h"
#include <iostream>

namespace nota {

std::string Generator::generate(const std::shared_ptr<Program>& program) {
    // 1. Register definitions
    definitions.clear();
    std::shared_ptr<Component> appRoot = nullptr;

    for (const auto& comp : program->components) {
        if (comp->type == "Item") {
            if (!comp->name.empty()) {
                definitions[comp->name] = comp;
            }
        } else if (comp->type == "App") {
            appRoot = comp;
        }
    }

    if (!appRoot) {
        return ""; // Or throw error
    }

    // 2. Generate App
    return generate(appRoot);
}

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

std::shared_ptr<Component> Generator::expandComponent(const std::shared_ptr<Component>& instance) {
    auto it = definitions.find(instance->type);
    if (it == definitions.end()) {
        return instance;
    }

    auto def = it->second;
    auto expanded = std::make_shared<Component>(instance->type, "");

    // Copy properties from definition
    for (const auto& prop : def->properties) {
        expanded->addProperty(prop);
    }

    // Override with instance properties
    for (const auto& prop : instance->properties) {
        expanded->addProperty(prop);
    }

    // Copy children from definition
    for (const auto& child : def->children) {
        expanded->addChild(child);
    }

    // Append instance children
    for (const auto& child : instance->children) {
        expanded->addChild(child);
    }

    return expanded;
}

void Generator::generateComponent(const std::shared_ptr<Component>& comp) {
    auto actualComp = expandComponent(comp);

    std::string className;
    std::string tag = "div";
    std::string textContent = "";

    if (actualComp->type == "App") {
        className = "nota-app";
        tag = "body";
    } else if (actualComp->type == "Text") {
        tag = "span";
        className = "nota-text-" + std::to_string(++counter);

        for (const auto& prop : actualComp->properties) {
            if (prop->name == "text") {
                 if (auto lit = std::dynamic_pointer_cast<Literal>(prop->value)) {
                     if (std::holds_alternative<std::string>(lit->value)) {
                         textContent = std::get<std::string>(lit->value);
                     }
                 }
            }
        }
    } else {
        className = "nota-" + actualComp->type + "-" + std::to_string(++counter);
    }

    css << "." << className << " {\n";
    css << generateStyles(actualComp, className);
    css << "}\n";

    html << "<" << tag << " class=\"" << className << "\"";

    for (const auto& prop : actualComp->properties) {
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

    if (!actualComp->children.empty()) {
        html << "\n";
        for (const auto& child : actualComp->children) {
            generateComponent(child);
        }
    }

    html << "</" << tag << ">\n";
}

std::string Generator::generateStyles(const std::shared_ptr<Component>& comp, const std::string& className) {
    std::stringstream ss;

    if (comp->type == "Row") {
        ss << "display: flex;\nflex-direction: row;\n";
    } else if (comp->type == "Col") {
        ss << "display: flex;\nflex-direction: column;\n";
    } else {
        ss << "display: block;\n";
    }

    // Default relative positioning to support absolute children
    ss << "position: relative;\n";

    bool hasPosition = false;
    for (const auto& prop : comp->properties) {
        if (prop->name == "position") hasPosition = true;
    }

    for (const auto& prop : comp->properties) {
        std::string val = getValue(prop->value);
        if (prop->name == "width") {
            ss << "width: " << val << ";\n";
        } else if (prop->name == "height") {
            ss << "height: " << val << ";\n";
        } else if (prop->name == "x") {
            ss << "left: " << val << ";\nposition: absolute;\n";
        } else if (prop->name == "y") {
            ss << "top: " << val << ";\nposition: absolute;\n";
        } else if (prop->name == "index") {
            // index (z-index) should not have px
            ss << "z-index: " << getValue(prop->value, false) << ";\n";
        } else if (prop->name == "position") {
            // Nota "position" property logic
            ss << "position: absolute;\n";
            if (val.find("center") != std::string::npos) {
                 if (val == "center") {
                     ss << "left: 50%; top: 50%; transform: translate(-50%, -50%);\n";
                 }
                 // Handle mixed like "left center" if needed, but for MVP sticking to basics
            }
            if (val.find("left") != std::string::npos) ss << "left: 0;\n";
            if (val.find("right") != std::string::npos) ss << "right: 0;\n";
            if (val.find("top") != std::string::npos) ss << "top: 0;\n";
            if (val.find("bottom") != std::string::npos) ss << "bottom: 0;\n";
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

std::string Generator::getValue(const std::shared_ptr<Expression>& expr, bool addPx) {
    if (auto lit = std::dynamic_pointer_cast<Literal>(expr)) {
        if (std::holds_alternative<int>(lit->value)) {
             return std::to_string(std::get<int>(lit->value)) + (addPx ? "px" : "");
        } else if (std::holds_alternative<double>(lit->value)) {
             return std::to_string(std::get<double>(lit->value)) + (addPx ? "px" : "");
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
        return getValue(mem->object) + "." + mem->member;
    }
    return "";
}

}
