#include "CodeGenerator.h"
#include <iostream>
#include <cmath>
#include <algorithm>

// Helper to format double values for CSS
std::string format_double(double val, const std::string& property_name) {
    bool is_unitless = (property_name == "z-index");
    std::string unit = is_unitless ? "" : "px";

    if (std::fmod(val, 1.0) == 0.0) {
        return std::to_string(static_cast<int>(val)) + unit;
    }
    return std::to_string(val) + unit;
}

bool has_positional_properties(const ComponentNode& component) {
    return std::any_of(component.properties.begin(), component.properties.end(), [](const PropertyNode& prop) {
        return prop.name.text == "x" || prop.name.text == "y" || prop.name.text == "index";
    });
}

bool has_child_with_positional_properties(const ComponentNode& component) {
    return std::any_of(component.children.begin(), component.children.end(), [](const auto& child) {
        return has_positional_properties(*child);
    });
}

std::string CodeGenerator::generate(const RootNode& root) {
    if (!root.root_component) {
        return "";
    }

    const auto& app_component = *root.root_component;
    get_unique_class_name(app_component); // Pre-generate all class names.

    generate_css_for_component(app_component, class_map_.at(&app_component), nullptr);

    for (const auto& child : app_component.children) {
        visit(*child, &app_component, html_stream_);
    }

    std::stringstream output;
    output << "<!DOCTYPE html>\n";
    output << "<html>\n";
    output << "<head>\n";
    output << "<style>\n";
    output << "/* Basic Reset */\n";
    output << "body, div, span { margin: 0; padding: 0; box-sizing: border-box; overflow: hidden; }\n";
    output << css_stream_.str();
    output << "</style>\n";
    output << "</head>\n";
    output << "<body class=\"" << class_map_.at(&app_component) << "\">\n";
    output << html_stream_.str();
    output << "</body>\n";
    output << "</html>\n";

    return output.str();
}

void CodeGenerator::visit(const ComponentNode& component, const ComponentNode* parent, std::stringstream& html_builder) {
    std::string class_name = class_map_.at(&component);

    generate_css_for_component(component, class_name, parent);

    std::string tag = "div";
    if (component.type.text == "Text") tag = "span";

    html_builder << "<" << tag << " class=\"" << class_name << "\">";

    for (const auto& prop : component.properties) {
        if (prop.name.text == "text") {
             if (std::holds_alternative<LiteralNode>(prop.value)) {
                const auto& literal = std::get<LiteralNode>(prop.value);
                if (std::holds_alternative<std::string>(literal.value)) {
                    std::string text_val = std::get<std::string>(literal.value);
                    if (text_val.front() == '"' && text_val.back() == '"') {
                        html_builder << text_val.substr(1, text_val.length() - 2);
                    }
                }
            }
        }
    }

    for (const auto& child : component.children) {
        visit(*child, &component, html_builder);
    }

    html_builder << "</" << tag << ">";
}

void CodeGenerator::generate_css_for_component(const ComponentNode& component, const std::string& class_name, const ComponentNode* parent) {
    css_stream_ << "." << class_name << " {\n";

    if (component.type.text == "Row") css_stream_ << "    display: flex;\n    flex-direction: row;\n";
    else if (component.type.text == "Col") css_stream_ << "    display: flex;\n    flex-direction: column;\n";

    if (has_positional_properties(component)) css_stream_ << "    position: absolute;\n";
    if (has_child_with_positional_properties(component)) css_stream_ << "    position: relative;\n";

    for (const auto& prop : component.properties) {
        if (prop.name.text == "text") continue;

        std::string css_prop;
        if (prop.name.text == "color" && component.type.text != "Text") {
            css_prop = "background-color";
        } else {
            css_prop = to_css_property(std::string(prop.name.text));
        }

        std::string css_val = to_css_value(prop.value, css_prop);
        if (!css_prop.empty() && !css_val.empty()) {
            css_stream_ << "    " << css_prop << ": " << css_val << ";\n";
        }
    }
    css_stream_ << "}\n";
}

std::string CodeGenerator::to_css_property(const std::string& nota_property) {
    if (nota_property == "spacing") return "gap";
    if (nota_property == "radius") return "border-radius";
    if (nota_property == "x") return "left";
    if (nota_property == "y") return "top";
    if (nota_property == "index") return "z-index";
    return nota_property;
}

std::string CodeGenerator::to_css_value(const ASTValue& value, const std::string& property_name) {
    if (std::holds_alternative<LiteralNode>(value)) {
        const auto& literal_node = std::get<LiteralNode>(value);
        if (std::holds_alternative<double>(literal_node.value)) {
            return format_double(std::get<double>(literal_node.value), property_name);
        }
        if (std::holds_alternative<std::string>(literal_node.value)) {
            std::string str_val = std::get<std::string>(literal_node.value);
            if (str_val.front() == '"' && str_val.back() == '"') {
                return str_val.substr(1, str_val.length() - 2);
            }
            return str_val;
        }
    }
    return "";
}

std::string CodeGenerator::get_unique_class_name(const ComponentNode& component) {
    if (class_map_.count(&component)) {
        return class_map_.at(&component);
    }
    std::string name = std::string(component.type.text) + "-" + std::to_string(class_counter_++);
    class_map_[&component] = name;
    for(const auto& child : component.children) {
        get_unique_class_name(*child);
    }
    return name;
}
