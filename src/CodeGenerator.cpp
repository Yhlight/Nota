#include "CodeGenerator.h"
#include "AST/Stmt.h"
#include <map>

// Helper map for component to tag mapping
static const std::map<std::string, std::string> component_tag_map = {
    {"App", "body"},
    {"Row", "div"},
    {"Col", "div"},
    {"Rect", "div"},
    {"Text", "span"},
};

CodeGenerator::CodeGenerator(std::map<std::string, const ItemStmt*> custom_types,
                           const std::map<const PropertyStmt*, std::any>& results)
    : custom_types(custom_types), results(results) {}

CompilationResult CodeGenerator::generate(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        statement->accept(*this);
    }
    return {html_out.str(), css_out.str()};
}

bool has_positioning(const std::vector<std::unique_ptr<Stmt>>& body) {
    for (const auto& child_stmt : body) {
        if (auto* comp = dynamic_cast<ComponentStmt*>(child_stmt.get())) {
            for (const auto& prop_stmt : comp->body) {
                if (auto* prop = dynamic_cast<PropertyStmt*>(prop_stmt.get())) {
                    if (prop->name.lexeme == "x" || prop->name.lexeme == "y" || prop->name.lexeme == "position" || prop->name.lexeme == "index") {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

std::any CodeGenerator::visit(const ComponentStmt& stmt) {
    // This logic is getting complex. A better implementation might use a two-pass system.
    if (custom_types.count(stmt.name.lexeme)) {
        // Custom component instantiation
        const ItemStmt* item = custom_types.at(stmt.name.lexeme);
        const ComponentStmt* root_comp = nullptr;
        std::vector<const PropertyStmt*> final_props;
        std::map<std::string, bool> seen_props;

        for(const auto& s : stmt.body) {
            if(auto* p = dynamic_cast<PropertyStmt*>(s.get())) {
                final_props.push_back(p);
                seen_props[p->name.lexeme] = true;
            }
        }
        for(const auto& s : item->body) if(auto* c = dynamic_cast<ComponentStmt*>(s.get())) root_comp = c;
        for(const auto& s : item->body) {
            if(auto* p = dynamic_cast<PropertyStmt*>(s.get())) {
                if(!seen_props.count(p->name.lexeme)) final_props.push_back(p);
            }
        }
        if(root_comp) {
            for(const auto& s : root_comp->body) {
                if(auto* p = dynamic_cast<PropertyStmt*>(s.get())) {
                     if(!seen_props.count(p->name.lexeme)) final_props.push_back(p);
                }
            }
        }

        if(root_comp) {
            std::string class_name = "nota-" + root_comp->name.lexeme + "-" + std::to_string(class_counter++);
            std::string tag = component_tag_map.count(root_comp->name.lexeme) ? component_tag_map.at(root_comp->name.lexeme) : "div";

            html_out << "<" << tag << " class=\"" << class_name << "\">";
            css_out << "." << class_name << " {\n";

            css_out << "  box-sizing: border-box;\n  overflow: hidden;\n  margin: 0;\n  padding: 0;\n";
            if (root_comp->name.lexeme == "Row") css_out << "  display: flex;\n  flex-direction: row;\n";
            else if (root_comp->name.lexeme == "Col") css_out << "  display: flex;\n  flex-direction: column;\n";
            if (has_positioning(root_comp->body) || has_positioning(stmt.body)) css_out << "  position: relative;\n";

            bool has_pos = false;
            for(const auto* prop : final_props) {
                if(prop->name.lexeme == "x" || prop->name.lexeme == "y" || prop->name.lexeme == "position") has_pos = true;
                prop->accept(*this);
            }
            if(has_pos) css_out << "  position: absolute;\n";

            css_out << "}\n";

            for(const auto& s : root_comp->body) if(dynamic_cast<ComponentStmt*>(s.get())) s->accept(*this);
            for(const auto& s : stmt.body) if(dynamic_cast<ComponentStmt*>(s.get())) s->accept(*this);

            html_out << "</" << tag << ">";
        }
    } else {
        // Built-in component
        std::string class_name = "nota-" + stmt.name.lexeme + "-" + std::to_string(class_counter++);
        std::string tag = component_tag_map.count(stmt.name.lexeme) ? component_tag_map.at(stmt.name.lexeme) : "div";

        html_out << "<" << tag << " class=\"" << class_name << "\">";
        css_out << "." << class_name << " {\n";

        css_out << "  box-sizing: border-box;\n  overflow: hidden;\n  margin: 0;\n  padding: 0;\n";
        if (stmt.name.lexeme == "Row") css_out << "  display: flex;\n  flex-direction: row;\n";
        else if (stmt.name.lexeme == "Col") css_out << "  display: flex;\n  flex-direction: column;\n";
        if (has_positioning(stmt.body)) css_out << "  position: relative;\n";

        bool has_pos = false;
        for (const auto& child_stmt : stmt.body) {
            if (auto* p = dynamic_cast<PropertyStmt*>(child_stmt.get())) {
                 if(p->name.lexeme == "x" || p->name.lexeme == "y" || p->name.lexeme == "position") has_pos = true;
                child_stmt->accept(*this);
            }
        }
        if(has_pos) css_out << "  position: absolute;\n";

        css_out << "}\n";

        for (const auto& child_stmt : stmt.body) if (dynamic_cast<ComponentStmt*>(child_stmt.get())) child_stmt->accept(*this);

        html_out << "</" << tag << ">";
    }
    return {};
}

std::any CodeGenerator::visit(const PropertyStmt& stmt) {
    if (stmt.name.lexeme == "id") {
        return {};
    }
    std::string prop_name = stmt.name.lexeme;
    bool is_unitless = false;

    if (prop_name == "spacing") prop_name = "gap";
    else if (prop_name == "radius") prop_name = "border-radius";
    else if (prop_name == "x") prop_name = "left";
    else if (prop_name == "y") prop_name = "top";
    else if (prop_name == "index") {
        prop_name = "z-index";
        is_unitless = true;
    }

    css_out << "  " << prop_name << ": ";
    if (results.count(&stmt)) {
        std::any value = results.at(&stmt);
        if (value.type() == typeid(double)) {
            css_out << std::any_cast<double>(value);
            if(!is_unitless) css_out << "px";
        } else if (value.type() == typeid(std::string)) {
            css_out << std::any_cast<std::string>(value);
        }
    }
    css_out << ";\n";
    return {};
}

std::any CodeGenerator::visit(const ItemStmt& stmt) {
    // We don't generate any code for an Item definition itself,
    // only for its instantiation. The resolver handles storing it.
    return {};
}
