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

    if (comp->type == "App") {
        className = "nota-app";
        tag = "body";
    } else {
        className = "nota-" + comp->type + "-" + std::to_string(++counter);
        // Normalize type to lower case for class name?
        // Keeping it as is for now or simple transform
        // For simplicity, just use the counter driven name or type
    }

    css << "." << className << " {\n";
    css << generateStyles(comp, className);
    css << "}\n";

    html << "<" << tag << " class=\"" << className << "\">\n";

    for (const auto& child : comp->children) {
        generateComponent(child);
    }

    html << "</" << tag << ">\n";
}

std::string Generator::generateStyles(const std::shared_ptr<Component>& comp, const std::string& className) {
    std::stringstream ss;
    ss << "display: block;\n";
    if (comp->type == "App") {
        // App typically fills the screen
        // Nota.md says App => body class="nota-app"
        // And "width: 100%, height: 100%" in example
    }

    for (const auto& prop : comp->properties) {
        std::string val = getValue(prop->value);
        if (prop->name == "width") {
            ss << "width: " << val << ";\n";
        } else if (prop->name == "height") {
            ss << "height: " << val << ";\n";
        } else if (prop->name == "color") {
            ss << "background-color: " << val << ";\n";
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
    }
    return "";
}

}
