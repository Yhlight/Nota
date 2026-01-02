#include "CodeGenerator.h"
#include <iostream>
#include <cmath>

// Helper to format double values for CSS
std::string format_double(double val) {
    if (std::fmod(val, 1.0) == 0.0) {
        return std::to_string(static_cast<int>(val));
    }
    return std::to_string(val);
}

std::string CodeGenerator::generate(const RootNode& root) {
    if (!root.root_component) {
        return "";
    }

    std::string app_class_name = get_unique_class_name("App");
    generate_css_for_component(*root.root_component, app_class_name);

    for (const auto& child : root.root_component->children) {
        visit(*child, html_stream_);
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
    output << "<body class=\"" << app_class_name << "\">\n";
    output << html_stream_.str();
    output << "</body>\n";
    output << "</html>\n";

    return output.str();
}

void CodeGenerator::visit(const ComponentNode& component, std::stringstream& html_builder) {
    std::string class_name = get_unique_class_name(std::string(component.type.text));

    generate_css_for_component(component, class_name);

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
        visit(*child, html_builder);
    }

    html_builder << "</" << tag << ">";
}

void CodeGenerator::generate_css_for_component(const ComponentNode& component, const std::string& class_name) {
    css_stream_ << "." << class_name << " {\n";
    if (component.type.text == "Row") {
        css_stream_ << "    display: flex;\n";
        css_stream_ << "    flex-direction: row;\n";
    } else if (component.type.text == "Col") {
        css_stream_ << "    display: flex;\n";
        css_stream_ << "    flex-direction: column;\n";
    }

    for (const auto& prop : component.properties) {
        if (prop.name.text == "text") continue;

        std::string css_prop;
        if (prop.name.text == "color" && component.type.text != "Text") {
            css_prop = "background-color";
        } else {
            css_prop = to_css_property(std::string(prop.name.text));
        }

        std::string css_val = to_css_value(prop.value);
        if (!css_prop.empty() && !css_val.empty()) {
            css_stream_ << "    " << css_prop << ": " << css_val << ";\n";
        }
    }
    css_stream_ << "}\n";
}

std::string CodeGenerator::to_css_property(const std::string& nota_property) {
    if (nota_property == "spacing") return "gap";
    if (nota_property == "radius") return "border-radius";
    return nota_property;
}

std::string CodeGenerator::to_css_value(const ASTValue& value) {
    if (std::holds_alternative<LiteralNode>(value)) {
        const auto& literal_node = std::get<LiteralNode>(value);
        if (std::holds_alternative<double>(literal_node.value)) {
            return format_double(std::get<double>(literal_node.value)) + "px";
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

std::string CodeGenerator::get_unique_class_name(const std::string& base_name) {
    return base_name + "-" + std::to_string(class_counter_++);
}
