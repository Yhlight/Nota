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
    std::string old_component_id = current_component_id;
    for (const auto& prop_stmt : stmt.body) {
        if (auto* prop = dynamic_cast<PropertyStmt*>(prop_stmt.get())) {
            if (prop->name.lexeme == "id") {
                if (auto* id_expr = dynamic_cast<IdentifierExpr*>(prop->value.get())) {
                    current_component_id = id_expr->name.lexeme;
                }
            }
        }
    }
    if (current_component_id.empty()) {
        current_component_id = "comp" + std::to_string(component_props.size());
    }

    for (const auto& prop_stmt : stmt.body) {
        prop_stmt->accept(*this);
    }

    for (const auto& child_stmt : stmt.body) {
        if (dynamic_cast<ComponentStmt*>(child_stmt.get())) {
            child_stmt->accept(*this);
        }
    }
    current_component_id = old_component_id;
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
    // This is a simplification. A real implementation would handle scopes.
    if(component_props.count(expr.name.lexeme)) {
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
    if (auto* obj = dynamic_cast<IdentifierExpr*>(expr.object.get())) {
        if(component_props.count(obj->name.lexeme)) {
            return component_props[obj->name.lexeme][expr.name.lexeme];
        }
    }
    throw std::runtime_error("Invalid get expression.");
}
