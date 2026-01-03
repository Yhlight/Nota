#include "CodeGenerator.h"
#include "codegen/ExpressionEvaluator.h"
#include "ast/ASTUtils.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <variant>
#include <unordered_map>
#include <map>

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
    evaluator_ = std::make_unique<ExpressionEvaluator>(component_map_, root_instance.get());
    evaluator_->build_parent_map(root_instance.get(), nullptr);
    apply_assignments(root);

    // Third pass: generate code
    std::string root_class = get_or_create_class_for_component(*root_instance, nullptr, nullptr);
    class_map_[root_instance.get()] = root_class;

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
    output << "<body class=\"" << root_class << "\">\n";
    output << html_stream_.str();

    // Fourth pass: generate scripts
    generate_scripts(root);
    if (!script_stream_.str().empty()) {
        output << "<script>\n";
        output << "document.addEventListener('DOMContentLoaded', function() {\n";
        output << script_stream_.str();
        output << "});\n";
        output << "</script>\n";
    }

    output << "</body>\n";
    output << "</html>\n";

    return output.str();
}

void CodeGenerator::apply_assignments(const RootNode& root) {
    if (!root.root_component) return;

    ExpressionEvaluator evaluator(component_map_, component_map_.at(root.root_component.get()));
    SymbolTable table;

    std::function<void(const ComponentNode&, ComponentNode*)> traverse =
        [&](const ComponentNode& original, ComponentNode* instance) {
        for (const auto& assignment : original.assignments) {
            Expression* target_expr = assignment.target.get();
            if (auto* member_access = std::get_if<MemberAccessNode>(&target_expr->variant)) {
                ComponentNode* target_component = evaluator.evaluate_target(*member_access->object, table, instance);
                if (target_component) {
                    std::string prop_name = std::string(member_access->member.text);
                    bool found = false;
                    for (auto& prop : target_component->properties) {
                        if (prop.name.text == prop_name) {
                            prop.value = evaluator.evaluate_value(assignment.value, table, instance);
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        // Report error, do not add new properties implicitly
                        errors_.push_back({ "Property '" + prop_name + "' not found on component '" + std::string(target_component->type.text) + "'.", member_access->member.line, member_access->member.column });
                    }
                }
            } else {
                // This case should be handled by the parser, but as a fallback:
                errors_.push_back({ "Invalid assignment expression.", assignment.line, assignment.column });
            }
        }

        for (size_t i = 0; i < original.children.size(); ++i) {
            traverse(*original.children[i], instance->children[i].get());
        }
    };

    traverse(*root.root_component, component_map_.at(root.root_component.get()));

    // Collect errors from the evaluator
    errors_.insert(errors_.end(), evaluator.errors().begin(), evaluator.errors().end());
}

void CodeGenerator::generate_component(const ComponentNode& component, const ComponentNode* parent, std::stringstream& html_builder, const std::vector<const PropertyNode*>* overridden_properties) {
    std::string type_name(component.type.text);
    if (item_definitions_.count(type_name)) {
        const ItemNode* item_def = item_definitions_.at(type_name);
        generate_item_component(*item_def, component, parent, html_builder);
    } else {
        std::string generated_class = get_or_create_class_for_component(component, parent, overridden_properties);
        std::string custom_class;

        for (const auto& prop : component.properties) {
            if (prop.name.text == "class") {
                if (auto val = to_css_value(prop.value, "class", const_cast<ComponentNode*>(&component))) {
                    if (holds_alternative<std::string>(*val)) {
                        custom_class = std::get<std::string>(*val);
                    }
                }
                break;
            }
        }

        std::string final_class = generated_class;
        if (!custom_class.empty()) {
            final_class += " " + custom_class;
        }

        std::string tag = "div";
        if (component.type.text == "Text") tag = "span";

        std::string id_attr;
        if (!component.event_handlers.empty()) {
            id_attr = " id=\"" + get_or_assign_id(component) + "\"";
        } else {
            for (const auto& prop : component.properties) {
                if (prop.name.text == "id") {
                    if (auto val = to_css_value(prop.value, "id", const_cast<ComponentNode*>(&component))) {
                         if (holds_alternative<std::string>(*val)) {
                            id_attr = " id=\"" + std::get<std::string>(*val) + "\"";
                        }
                    }
                    break;
                }
            }
        }

        html_builder << "<" << tag << " class=\"" << final_class << "\"" << id_attr << ">";

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
    std::unordered_map<std::string, const PropertyNode*> property_map;
    for (const auto& prop : item_def.properties) {
        property_map[std::string(prop.name.text)] = &prop;
    }
    for (const auto& prop : instance.properties) {
        property_map[std::string(prop.name.text)] = &prop;
    }

    std::vector<const PropertyNode*> merged_properties;
    for (const auto& pair : property_map) {
        merged_properties.push_back(pair.second);
    }

    for (const auto& child : item_def.children) {
        generate_component(*child, parent, html_builder, &merged_properties);
    }
}

std::string CodeGenerator::get_or_create_class_for_component(const ComponentNode& component, const ComponentNode* parent, const std::vector<const PropertyNode*>* overridden_properties) {
    std::string style_key = generate_css_properties_string(component, parent, overridden_properties);

    if (style_to_class_map_.count(style_key)) {
        return style_to_class_map_.at(style_key);
    }

    std::string new_class_name = std::string(component.type.text) + "-" + std::to_string(class_counter_++);
    style_to_class_map_[style_key] = new_class_name;

    css_stream_ << "." << new_class_name << " {\n" << style_key << "}\n";

    return new_class_name;
}

std::string CodeGenerator::generate_css_properties_string(const ComponentNode& component, const ComponentNode* parent, const std::vector<const PropertyNode*>* overridden_properties) {
    std::stringstream props_stream;

    if (component.type.text == "Row") props_stream << "    display: flex;\n    flex-direction: row;\n";
    else if (component.type.text == "Col") props_stream << "    display: flex;\n    flex-direction: column;\n";

    bool has_position = false;
    for (const auto& prop : component.properties) {
        if (prop.name.text == "position") {
            has_position = true;
            break;
        }
    }

    if (has_positional_properties(component) || has_position) {
        props_stream << "    position: absolute;\n";
    }
    if (has_child_with_positional_properties(component) || std::any_of(component.children.begin(), component.children.end(), [](const auto& child) {
        return std::any_of(child->properties.begin(), child->properties.end(), [](const auto& prop) {
            return prop.name.text == "position";
        });
    })) {
        props_stream << "    position: relative;\n";
    }

    std::map<std::string, const PropertyNode*> property_map; // Use map for sorting
    for (const auto& prop : component.properties) {
        property_map[std::string(prop.name.text)] = &prop;
    }

    if (overridden_properties) {
        for (const auto* prop : *overridden_properties) {
            property_map[std::string(prop->name.text)] = prop;
        }
    }

    for (const auto& pair : property_map) {
        const auto* prop = pair.second;
        if (prop->name.text == "text" || prop->name.text == "class" || prop->name.text == "id") continue;

        std::string css_prop;
        if (prop->name.text == "color" && component.type.text != "Text") {
            css_prop = "background-color";
        } else if (prop->name.text == "position") {
            // Special handling for position, no direct css property
        } else {
            css_prop = to_css_property(std::string(prop->name.text));
        }

        auto evaluated_value = to_css_value(prop->value, css_prop, const_cast<ComponentNode*>(&component));

        if (evaluated_value) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    props_stream << "    " << css_prop << ": " << format_double(arg, css_prop) << ";\n";
                } else if constexpr (std::is_same_v<T, std::string>) {
                    props_stream << "    " << css_prop << ": " << arg << ";\n";
                } else if constexpr (std::is_same_v<T, PositionNode>) {
                    std::string first = std::string(arg.first.keyword.text);
                    std::string second = arg.second ? std::string(arg.second->keyword.text) : "";

                    if (first == "center") {
                        props_stream << "    left: 50%;\n";
                        props_stream << "    top: 50%;\n";
                        props_stream << "    transform: translate(-50%, -50%);\n";
                    } else {
                        if (first == "left" || second == "left") props_stream << "    left: 0;\n";
                        if (first == "right" || second == "right") props_stream << "    right: 0;\n";
                        if (first == "top" || second == "top") props_stream << "    top: 0;\n";
                        if (first == "bottom" || second == "bottom") props_stream << "    bottom: 0;\n";
                    }
                }
            }, *evaluated_value);
        }
    }
    return props_stream.str();
}


std::string CodeGenerator::to_css_property(const std::string& nota_property) {
    if (nota_property == "spacing") return "gap";
    if (nota_property == "radius") return "border-radius";
    if (nota_property == "x") return "left";
    if (nota_property == "y") return "top";
    if (nota_property == "index") return "z-index";
    return nota_property;
}

std::optional<CodeGenerator::EvaluatedValue> CodeGenerator::to_css_value(const ASTValue& value, const std::string& property_name, ComponentNode* current_component) {
    ASTValue evaluated_value;
    const ASTValue* value_to_process = &value;

    if (const auto* expr = std::get_if<std::unique_ptr<Expression>>(&value)) {
        SymbolTable table;
        evaluated_value = evaluator_->evaluate_expression(**expr, table, current_component);
        value_to_process = &evaluated_value;
    }

    if (const auto* literal = std::get_if<LiteralNode>(value_to_process)) {
        if (const auto* number = std::get_if<double>(&literal->value)) {
            return *number;
        }
        if (const auto* str_val = std::get_if<std::string>(&literal->value)) {
            if (!str_val->empty() && str_val->front() == '"' && str_val->back() == '"') {
                return str_val->substr(1, str_val->length() - 2);
            }
            return *str_val;
        }
    } else if (const auto* pos_node = std::get_if<PositionNode>(value_to_process)) {
        return *pos_node;
    }
    return std::nullopt;
}

std::string CodeGenerator::get_or_assign_id(const ComponentNode& component) {
    if (id_map_.count(&component)) {
        return id_map_.at(&component);
    }

    // Check for user-defined 'id' property first
    for (const auto& prop : component.properties) {
        if (prop.name.text == "id") {
             if (auto val = to_css_value(prop.value, "id", const_cast<ComponentNode*>(&component))) {
                if (holds_alternative<std::string>(*val)) {
                    std::string user_id = std::get<std::string>(*val);
                    id_map_[&component] = user_id;
                    return user_id;
                }
            }
        }
    }

    std::string new_id = "nota-comp-" + std::to_string(id_counter_++);
    id_map_[&component] = new_id;
    return new_id;
}

void CodeGenerator::generate_scripts(const RootNode& root) {
    if (root.root_component) {
        find_and_generate_scripts(*root.root_component);
    }
}

void CodeGenerator::find_and_generate_scripts(const ComponentNode& component) {
    if (!component.event_handlers.empty()) {
        std::string id = get_or_assign_id(component);
        std::string var_name = "comp" + std::to_string(script_var_counter_++);
        script_stream_ << "const " << var_name << " = document.getElementById('" << id << "');\n";
        script_stream_ << "if (" << var_name << ") {\n";
        for (const auto& handler : component.event_handlers) {
            std::string event_name = std::string(handler.name.text);
            if (event_name == "onClick") {
                script_stream_ << "  " << var_name << ".addEventListener('click', function() {\n";
                script_stream_ << "    " << handler.body << "\n";
                script_stream_ << "  });\n";
            } else if (event_name == "onHover") {
                script_stream_ << "  " << var_name << ".addEventListener('mouseover', function() {\n";
                script_stream_ << "    " << handler.body << "\n";
                script_stream_ << "  });\n";
            }
        }
        script_stream_ << "}\n";
    }

    for (const auto& child : component.children) {
        find_and_generate_scripts(*child);
    }
}
