#include "Evaluator.h"
#include <stdexcept>

void Evaluator::evaluate(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        statement->accept(*this);
    }
}

const std::map<const PropertyStmt*, std::any>& Evaluator::get_results() const {
    return results;
}

std::any Evaluator::visit(const ComponentStmt& stmt) {
    std::string component_id;
    for (const auto& prop_stmt : stmt.body) {
        if (auto* prop = dynamic_cast<PropertyStmt*>(prop_stmt.get())) {
            if (prop->name.lexeme == "id") {
                if (auto* id_expr = dynamic_cast<IdentifierExpr*>(prop->value.get())) {
                    component_id = id_expr->name.lexeme;
                }
            }
        }
    }
    if (component_id.empty()) {
        component_id = "comp" + std::to_string(component_props.size());
    }

    current_component_id = component_id;
    component_stack.push(component_id);

    for (const auto& prop_stmt : stmt.body) {
        prop_stmt->accept(*this);
    }

    for (const auto& child_stmt : stmt.body) {
        if (dynamic_cast<ComponentStmt*>(child_stmt.get())) {
            child_stmt->accept(*this);
        }
    }

    component_stack.pop();
    if (!component_stack.empty()) {
        current_component_id = component_stack.top();
    } else {
        current_component_id = "";
    }
    return {};
}

std::any Evaluator::visit(const PropertyStmt& stmt) {
    std::any value = stmt.value->accept(*this);
    component_props[current_component_id][stmt.name.lexeme] = value;
    results[&stmt] = value;
    return value;
}

std::any Evaluator::visit(const ItemStmt& stmt) {
    for (const auto& s : stmt.body) {
        s->accept(*this);
    }
    return {};
}

std::any Evaluator::visit(const LiteralExpr& expr) {
    return expr.value;
}

std::any Evaluator::visit(const IdentifierExpr& expr) {
    if (expr.name.lexeme == "parent") {
        if (component_stack.size() > 1) {
            // Fragile: relies on the stack having at least 2 elements.
            const auto& current_id = component_stack.top();
            component_stack.pop(); // Pop temporarily
            const auto& parent_id = component_stack.top();
            component_stack.push(current_id); // Push back

            if (component_props.count(parent_id)) {
                return component_props[parent_id];
            }
        }
        throw std::runtime_error("Cannot access 'parent' in this context.");
    }

    if (component_props.count(expr.name.lexeme)) {
        return component_props[expr.name.lexeme];
    }
    return {};
}

std::any Evaluator::visit(const BinaryExpr& expr) {
    std::any left = expr.left->accept(*this);
    std::any right = expr.right->accept(*this);

    if (left.type() == typeid(double) && right.type() == typeid(double)) {
        double left_d = std::any_cast<double>(left);
        double right_d = std::any_cast<double>(right);
        switch (expr.op.type) {
            case TokenType::PLUS: return left_d + right_d;
            case TokenType::MINUS: return left_d - right_d;
            case TokenType::SLASH: return left_d / right_d;
            case TokenType::STAR: return left_d * right_d;
            case TokenType::PERCENT: return (double)((int)left_d % (int)right_d);
            default: break;
        }
    }

    throw std::runtime_error("Unsupported binary operation.");
}

std::any Evaluator::visit(const GetExpr& expr) {
    std::any object = expr.object->accept(*this);

    if (object.type() == typeid(std::map<std::string, std::any>)) {
        const auto& props = std::any_cast<std::map<std::string, std::any>>(object);
        if (props.count(expr.name.lexeme)) {
            return props.at(expr.name.lexeme);
        }
    }

    throw std::runtime_error("Invalid property access.");
}
