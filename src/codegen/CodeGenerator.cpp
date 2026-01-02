#include "CodeGenerator.h"
#include "codegen/ExpressionEvaluator.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <variant>

// Forward declarations
std::unique_ptr<ComponentNode> deep_copy(const ComponentNode& node);
std::unique_ptr<Expression> deep_copy_expression(const Expression& expr);
ASTValue deep_copy_ast_value(const ASTValue& value);

// Implementation of deep_copy_expression
std::unique_ptr<Expression> deep_copy_expression(const Expression& expr) {
    auto new_expr = std::make_unique<Expression>();
    new_expr->variant = std::visit(
        [&](auto&& arg) -> ExpressionVariant {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, LiteralNode>) {
                return arg; // LiteralNode is copyable
            } else if constexpr (std::is_same_v<T, MemberAccessNode>) {
                return MemberAccessNode{deep_copy_expression(*arg.object), arg.member};
            } else if constexpr (std::is_same_v<T, IndexAccessNode>) {
                return IndexAccessNode{deep_copy_expression(*arg.object), deep_copy_expression(*arg.index)};
            }
        },
        expr.variant
    );
    return new_expr;
}

// Implementation of deep_copy_ast_value
ASTValue deep_copy_ast_value(const ASTValue& value) {
    return std::visit(
        [&](auto&& arg) -> ASTValue {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, LiteralNode>) {
                return arg; // LiteralNode is copyable
            } else if constexpr (std::is_same_v<T, std::unique_ptr<ComponentNode>>) {
                return deep_copy(*arg);
            } else if constexpr (std::is_same_v<T, std::unique_ptr<Expression>>) {
                return deep_copy_expression(*arg);
            }
        },
        value
    );
}

// Implementation of deep_copy for ComponentNode
std::unique_ptr<ComponentNode> deep_copy(const ComponentNode& node) {
    auto new_node = std::make_unique<ComponentNode>();
    new_node->type = node.type;
    for (const auto& prop : node.properties) {
        PropertyNode new_prop;
        new_prop.name = prop.name;
        new_prop.value = deep_copy_ast_value(prop.value);
        new_node->properties.push_back(std::move(new_prop));
    }
    for (const auto& child : node.children) {
        new_node->children.push_back(deep_copy(*child));
    }
    for (const auto& assignment : node.assignments) {
        AssignmentNode new_assignment;
        new_assignment.target = deep_copy_expression(*assignment.target);
        new_assignment.value = deep_copy_ast_value(assignment.value);
        new_node->assignments.push_back(std::move(new_assignment));
    }
    return new_node;
}

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

    for (const auto& item_def : root.item_definitions) {
        item_definitions_[std::string(item_def->name.text)] = item_def.get();
    }

    // First pass: build the component tree
    auto root_instance = deep_copy(*root.root_component);
    component_map_[root.root_component.get()] = root_instance.get();

    std::function<void(const ComponentNode&, ComponentNode&)> build_tree =
        [&](const ComponentNode& original, ComponentNode& instance) {
        for (size_t i = 0; i < original.children.size(); ++i) {
            component_map_[original.children[i].get()] = instance.children[i].get();
            build_tree(*original.children[i], *instance.children[i]);
        }
    };
    build_tree(*root.root_component, *root_instance);

    // Second pass: apply assignments
    apply_assignments(root);

    // Third pass: generate code
    get_unique_class_name(*root_instance);
    generate_css_for_component(*root_instance, class_map_.at(root_instance.get()), nullptr, nullptr);

    for (const auto& child : root_instance->children) {
        generate_component(*child, root_instance.get(), html_stream_);
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
    output << "<body class=\"" << class_map_.at(root_instance.get()) << "\">\n";
    output << html_stream_.str();
    output << "</body>\n";
    output << "</html>\n";

    return output.str();
}

void CodeGenerator::apply_assignments(const RootNode& root) {
    if (!root.root_component) return;

    ExpressionEvaluator evaluator(component_map_);
    SymbolTable table;

    std::function<void(const ComponentNode&)> traverse = [&](const ComponentNode& original) {
        ComponentNode* instance = component_map_.at(&original);
        for (const auto& assignment : original.assignments) {
            ComponentNode* target = evaluator.evaluate_target(*assignment.target, table, instance);
            if (target) {
                if (auto* member_access = std::get_if<MemberAccessNode>(&assignment.target->variant)) {
                    std::string prop_name = std::string(member_access->member.text);
                    bool found = false;
                    for (auto& prop : target->properties) {
                        if (prop.name.text == prop_name) {
                            prop.value = evaluator.evaluate_value(assignment.value, table);
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        PropertyNode new_prop;
                        new_prop.name = member_access->member;
                        new_prop.value = evaluator.evaluate_value(assignment.value, table);
                        target->properties.push_back(std::move(new_prop));
                    }
                }
            }
        }
        for (const auto& child : original.children) {
            traverse(*child);
        }
    };
    traverse(*root.root_component);
}

void CodeGenerator::generate_component(const ComponentNode& component, const ComponentNode* parent, std::stringstream& html_builder, const std::vector<const PropertyNode*>* overridden_properties) {
    std::string type_name(component.type.text);
    if (item_definitions_.count(type_name)) {
        const ItemNode* item_def = item_definitions_.at(type_name);
        generate_item_component(*item_def, component, parent, html_builder);
    } else {
        std::string class_name = get_unique_class_name(component);
        generate_css_for_component(component, class_name, parent, overridden_properties);

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
            generate_component(*child, &component, html_builder);
        }

        html_builder << "</" << tag << ">";
    }
}

void CodeGenerator::generate_item_component(const ItemNode& item_def, const ComponentNode& instance, const ComponentNode* parent, std::stringstream& html_builder) {
    std::vector<const PropertyNode*> merged_properties;
    for (const auto& prop : item_def.properties) {
        merged_properties.push_back(&prop);
    }
    for (const auto& prop : instance.properties) {
        bool overridden = false;
        for (auto& merged_prop : merged_properties) {
            if (merged_prop->name.text == prop.name.text) {
                merged_prop = &prop;
                overridden = true;
                break;
            }
        }
        if (!overridden) {
            merged_properties.push_back(&prop);
        }
    }

    for (const auto& child : item_def.children) {
        generate_component(*child, parent, html_builder, &merged_properties);
    }
}

void CodeGenerator::generate_css_for_component(const ComponentNode& component, const std::string& class_name, const ComponentNode* parent, const std::vector<const PropertyNode*>* overridden_properties) {
    css_stream_ << "." << class_name << " {\n";

    if (component.type.text == "Row") css_stream_ << "    display: flex;\n    flex-direction: row;\n";
    else if (component.type.text == "Col") css_stream_ << "    display: flex;\n    flex-direction: column;\n";

    if (has_positional_properties(component)) css_stream_ << "    position: absolute;\n";
    if (has_child_with_positional_properties(component)) css_stream_ << "    position: relative;\n";

    std::vector<const PropertyNode*> properties_to_generate;
    for (const auto& prop : component.properties) {
        properties_to_generate.push_back(&prop);
    }

    if (overridden_properties) {
        for (const auto* prop : *overridden_properties) {
            bool overridden = false;
            for (auto*& existing_prop : properties_to_generate) {
                if (existing_prop->name.text == prop->name.text) {
                    existing_prop = prop;
                    overridden = true;
                    break;
                }
            }
            if (!overridden) {
                properties_to_generate.push_back(prop);
            }
        }
    }

    for (const auto* prop : properties_to_generate) {
        if (prop->name.text == "text") continue;

        std::string css_prop;
        if (prop->name.text == "color" && component.type.text != "Text") {
            css_prop = "background-color";
        } else {
            css_prop = to_css_property(std::string(prop->name.text));
        }

        std::string css_val = to_css_value(prop->value, css_prop);
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
    if (auto* literal = std::get_if<LiteralNode>(&value)) {
        if (auto* number = std::get_if<double>(&literal->value)) {
            return format_double(*number, property_name);
        }
        if (auto* str_val = std::get_if<std::string>(&literal->value)) {
             if (str_val->front() == '"' && str_val->back() == '"') {
                return str_val->substr(1, str_val->length() - 2);
            }
            return *str_val;
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
