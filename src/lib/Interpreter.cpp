#include "Interpreter.h"
#include <stdexcept>

static double toDouble(const std::variant<std::monostate, int, double, std::string, bool>& v) {
    if (std::holds_alternative<int>(v)) {
        return static_cast<double>(std::get<int>(v));
    } else if (std::holds_alternative<double>(v)) {
        return std::get<double>(v);
    }
    throw std::runtime_error("Operand must be a number.");
}

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
            } else {
                last_value = toDouble(left) + toDouble(right);
            }
            break;
        case TokenType::MINUS:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                last_value = std::get<int>(left) - std::get<int>(right);
            } else {
                last_value = toDouble(left) - toDouble(right);
            }
            break;
        case TokenType::STAR:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                last_value = std::get<int>(left) * std::get<int>(right);
            } else {
                last_value = toDouble(left) * toDouble(right);
            }
            break;
        case TokenType::SLASH:
            last_value = toDouble(left) / toDouble(right);
            break;
        case TokenType::GREATER:
            last_value = toDouble(left) > toDouble(right);
            break;
        case TokenType::GREATER_EQUAL:
            last_value = toDouble(left) >= toDouble(right);
            break;
        case TokenType::LESS:
            last_value = toDouble(left) < toDouble(right);
            break;
        case TokenType::LESS_EQUAL:
            last_value = toDouble(left) <= toDouble(right);
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

void Interpreter::visit(const Variable& expr) {
    last_value = environment->get(expr.name);
}

void Interpreter::visit(const Assign& expr) {
    evaluate(expr.value);
    environment->assign(expr.name, last_value);
}

void Interpreter::visit(const Postfix& expr) {
    if (auto var = std::dynamic_pointer_cast<Variable>(expr.left)) {
        auto current_value = environment->get(var->name);
        last_value = current_value; // The result of a postfix expression is the value *before* the operation.

        if (std::holds_alternative<int>(current_value)) {
            int int_val = std::get<int>(current_value);
            if (expr.op.type == TokenType::PLUS_PLUS) {
                environment->assign(var->name, int_val + 1);
            } else {
                environment->assign(var->name, int_val - 1);
            }
        } else if (std::holds_alternative<double>(current_value)) {
            double double_val = std::get<double>(current_value);
            if (expr.op.type == TokenType::PLUS_PLUS) {
                environment->assign(var->name, double_val + 1.0);
            } else {
                environment->assign(var->name, double_val - 1.0);
            }
        } else {
            throw std::runtime_error("Operand must be a number for postfix operators.");
        }
    } else {
        throw std::runtime_error("Invalid target for postfix operator.");
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

void Interpreter::visit(const BlockStmt& stmt) {
    executeBlock(stmt.statements, std::make_shared<Environment>(environment));
}

void Interpreter::visit(const IfStmt& stmt) {
    evaluate(stmt.condition);
    if (isTruthy(last_value)) {
        execute(stmt.thenBranch);
    } else if (stmt.elseBranch != nullptr) {
        execute(stmt.elseBranch);
    }
}

void Interpreter::visit(const WhileStmt& stmt) {
    evaluate(stmt.condition);
    while (isTruthy(last_value)) {
        execute(stmt.body);
        evaluate(stmt.condition);
    }
}

void Interpreter::visit(const ForStmt& stmt) {
    auto previous = this->environment;
    this->environment = std::make_shared<Environment>(previous);

    if (stmt.initializer != nullptr) {
        execute(stmt.initializer);
    }

    while (true) {
        if (stmt.condition != nullptr) {
            evaluate(stmt.condition);
            if (!isTruthy(last_value)) {
                break;
            }
        }

        execute(stmt.body);

        if (stmt.increment != nullptr) {
            evaluate(stmt.increment);
        }
    }
    this->environment = previous;
}

void Interpreter::visit(const DoWhileStmt& stmt) {
    do {
        execute(stmt.body);
        evaluate(stmt.condition);
    } while (isTruthy(last_value));
}

void Interpreter::execute(const std::shared_ptr<Stmt>& stmt) {
    stmt->accept(*this);
}

void Interpreter::executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment) {
    auto previous = this->environment;
    this->environment = environment;
    for (const auto& statement : statements) {
        execute(statement);
    }
    this->environment = previous;
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

    if ((std::holds_alternative<int>(a) || std::holds_alternative<double>(a)) && (std::holds_alternative<int>(b) || std::holds_alternative<double>(b))) {
        return toDouble(a) == toDouble(b);
    }

    return a == b;
}

std::shared_ptr<Environment> Interpreter::getGlobals() {
    return globals;
}
