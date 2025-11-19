#include "Interpreter.h"
#include "NotaCallable.h"
#include <stdexcept>
#include <variant>

static double toDouble(const std::any& v) {
    if (v.type() == typeid(int)) {
        return static_cast<double>(std::any_cast<int>(v));
    } else if (v.type() == typeid(double)) {
        return std::any_cast<double>(v);
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
            if (left.type() == typeid(int) && right.type() == typeid(int)) {
                last_value = std::any_cast<int>(left) + std::any_cast<int>(right);
            } else {
                last_value = toDouble(left) + toDouble(right);
            }
            break;
        case TokenType::MINUS:
            if (left.type() == typeid(int) && right.type() == typeid(int)) {
                last_value = std::any_cast<int>(left) - std::any_cast<int>(right);
            } else {
                last_value = toDouble(left) - toDouble(right);
            }
            break;
        case TokenType::STAR:
            if (left.type() == typeid(int) && right.type() == typeid(int)) {
                last_value = std::any_cast<int>(left) * std::any_cast<int>(right);
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
    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            last_value = std::any{};
        } else {
            last_value = arg;
        }
    }, expr.value);
}

void Interpreter::visit(const Unary& expr) {
    evaluate(expr.right);
    switch (expr.op.type) {
        case TokenType::MINUS:
            if (last_value.type() == typeid(int)) {
                last_value = -std::any_cast<int>(last_value);
            } else if (last_value.type() == typeid(double)) {
                last_value = -std::any_cast<double>(last_value);
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

        if (current_value.type() == typeid(int)) {
            int int_val = std::any_cast<int>(current_value);
            if (expr.op.type == TokenType::PLUS_PLUS) {
                environment->assign(var->name, int_val + 1);
            } else {
                environment->assign(var->name, int_val - 1);
            }
        } else if (current_value.type() == typeid(double)) {
            double double_val = std::any_cast<double>(current_value);
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

void Interpreter::visit(const Call& expr) {
    evaluate(expr.callee);
    std::any callee = last_value;

    std::vector<std::any> arguments;
    for (const auto& arg : expr.arguments) {
        evaluate(arg);
        arguments.push_back(last_value);
    }

    if (callee.type() != typeid(std::shared_ptr<NotaCallable>)) {
        throw std::runtime_error("Can only call functions and classes.");
    }

    auto function = std::any_cast<std::shared_ptr<NotaCallable>>(callee);
    if (arguments.size() != function->arity()) {
        throw std::runtime_error("Expected " + std::to_string(function->arity()) +
                                 " arguments but got " + std::to_string(arguments.size()) + ".");
    }
    last_value = function->call(*this, arguments);
}

void Interpreter::visit(const ExpressionStmt& stmt) {
    evaluate(stmt.expression);
}

void Interpreter::visit(const VarStmt& stmt) {
    std::any value;
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

void Interpreter::visit(const FunctionStmt& stmt) {
    auto function = std::make_shared<NotaFunction>(std::make_shared<const FunctionStmt>(stmt), environment);
    environment->define(stmt.name.lexeme, std::static_pointer_cast<NotaCallable>(function));
}

void Interpreter::visit(const ReturnStmt& stmt) {
    std::any value;
    if (stmt.value != nullptr) {
        evaluate(stmt.value);
        value = last_value;
    }
    throw Return(value);
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

bool Interpreter::isTruthy(const std::any& value) {
    if (!value.has_value()) return false;
    if (value.type() == typeid(bool)) return std::any_cast<bool>(value);
    return true;
}

bool Interpreter::isEqual(const std::any& a, const std::any& b) {
    if (!a.has_value() && !b.has_value()) return true;
    if (!a.has_value()) return false;

    if ((a.type() == typeid(int) || a.type() == typeid(double)) && (b.type() == typeid(int) || b.type() == typeid(double))) {
        return toDouble(a) == toDouble(b);
    }

    if (a.type() != b.type()) return false;

    if (a.type() == typeid(int)) return std::any_cast<int>(a) == std::any_cast<int>(b);
    if (a.type() == typeid(double)) return std::any_cast<double>(a) == std::any_cast<double>(b);
    if (a.type() == typeid(bool)) return std::any_cast<bool>(a) == std::any_cast<bool>(b);
    if (a.type() == typeid(std::string)) return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);

    return false;
}

std::any Interpreter::getLastValue() const {
    return last_value;
}
