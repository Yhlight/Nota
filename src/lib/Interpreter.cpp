#include "Interpreter.h"
#include <stdexcept>

Interpreter::Interpreter() {
    globals = std::make_shared<Environment>();
    environment = globals;
}

void Interpreter::interpret(const std::vector<std::shared_ptr<Stmt>>& statements) {
    try {
        for (const auto& statement : statements) {
            execute(statement);
        }
    } catch (const std::runtime_error& e) {
        // Handle error
    }
}

void Interpreter::visit(const Binary& expr) {
    evaluate(expr.left);
    auto left = last_value;
    evaluate(expr.right);
    auto right = last_value;

    switch (expr.op.type) {
        case TokenType::PLUS:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                last_value = std::get<int>(left) + std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_value = std::get<double>(left) + std::get<double>(right);
            }
            break;
        case TokenType::MINUS:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                last_value = std::get<int>(left) - std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_value = std::get<double>(left) - std::get<double>(right);
            }
            break;
        case TokenType::STAR:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                last_value = std::get<int>(left) * std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_value = std::get<double>(left) * std::get<double>(right);
            }
            break;
        case TokenType::SLASH:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                last_value = std::get<int>(left) / std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_value = std::get<double>(left) / std::get<double>(right);
            }
            break;
        case TokenType::GREATER:
            last_value = std::get<double>(left) > std::get<double>(right);
            break;
        case TokenType::GREATER_EQUAL:
            last_value = std::get<double>(left) >= std::get<double>(right);
            break;
        case TokenType::LESS:
            last_value = std::get<double>(left) < std::get<double>(right);
            break;
        case TokenType::LESS_EQUAL:
            last_value = std::get<double>(left) <= std::get<double>(right);
            break;
        case TokenType::BANG_EQUAL:
            last_value = !isEqual(left, right);
            break;
        case TokenType::EQUAL_EQUAL:
            last_value = isEqual(left, right);
            break;
        default:
            break;
    }
}

void Interpreter::visit(const Grouping& expr) {
    evaluate(expr.expression);
}

void Interpreter::visit(const Literal& expr) {
    last_value = expr.value;
}

void Interpreter::visit(const Unary& expr) {
    evaluate(expr.right);
    switch (expr.op.type) {
        case TokenType::MINUS:
            if (std::holds_alternative<int>(last_value)) {
                last_value = -std::get<int>(last_value);
            } else if (std::holds_alternative<double>(last_value)) {
                last_value = -std::get<double>(last_value);
            }
            break;
        case TokenType::BANG:
            last_value = !isTruthy(last_value);
            break;
        default:
            break;
    }
}

void Interpreter::visit(const ExpressionStmt& stmt) {
    evaluate(stmt.expression);
}

void Interpreter::visit(const VarStmt& stmt) {
    std::variant<std::monostate, int, double, std::string, bool> value;
    if (stmt.initializer != nullptr) {
        evaluate(stmt.initializer);
        value = last_value;
    }
    environment->define(stmt.name.lexeme, value, stmt.isMutable);
}

void Interpreter::execute(const std::shared_ptr<Stmt>& stmt) {
    stmt->accept(*this);
}

void Interpreter::evaluate(const std::shared_ptr<Expr>& expr) {
    expr->accept(*this);
}

bool Interpreter::isTruthy(const std::variant<std::monostate, int, double, std::string, bool>& value) {
    if (std::holds_alternative<std::monostate>(value)) return false;
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
    return true;
}

bool Interpreter::isEqual(const std::variant<std::monostate, int, double, std::string, bool>& a, const std::variant<std::monostate, int, double, std::string, bool>& b) {
    if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) return true;
    if (std::holds_alternative<std::monostate>(a)) return false;
    return a == b;
}

std::shared_ptr<Environment> Interpreter::getGlobals() {
    return globals;
}
