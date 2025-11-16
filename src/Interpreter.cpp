#include "Interpreter.h"
#include "RuntimeError.h"
#include <iostream>
#include <stdexcept>

Interpreter::Interpreter() {
    environment = std::make_shared<Environment>();
}


void Interpreter::interpret(const std::vector<std::unique_ptr<Stmt>>& statements) {
    try {
        for (const auto& statement : statements) {
            execute(*statement);
        }
    } catch (const RuntimeError& error) {
        // In a real interpreter, you'd report this to the user.
        std::cerr << error.what() << "\n[line " << error.token.line << "]" << std::endl;
    }
}

std::any Interpreter::evaluate(const Expr& expr) {
    return expr.accept(*this);
}

void Interpreter::execute(const Stmt& stmt) {
    stmt.accept(*this);
}


std::any Interpreter::visitLiteralExpr(const Literal& expr) {
    return expr.value;
}

std::any Interpreter::visitGroupingExpr(const Grouping& expr) {
    return evaluate(*expr.expression);
}

std::any Interpreter::visitUnaryExpr(const Unary& expr) {
    std::any right = evaluate(*expr.right);

    switch (expr.op.type) {
        case TokenType::MINUS:
            checkNumberOperand(expr.op, right);
            return -std::any_cast<double>(right);
        case TokenType::BANG:
            return !isTruthy(right);
        default:
            // Unreachable.
            return {};
    }
}

std::any Interpreter::visitVariableExpr(const Variable& expr) {
    return environment->get(expr.name);
}


std::any Interpreter::visitBinaryExpr(const Binary& expr) {
    std::any left = evaluate(*expr.left);
    std::any right = evaluate(*expr.right);

    switch (expr.op.type) {
        case TokenType::GREATER:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) > std::any_cast<double>(right);
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) >= std::any_cast<double>(right);
        case TokenType::LESS:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) < std::any_cast<double>(right);
        case TokenType::LESS_EQUAL:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) <= std::any_cast<double>(right);
        case TokenType::BANG_EQUAL:
            return !isEqual(left, right);
        case TokenType::EQUAL_EQUAL:
            return isEqual(left, right);
        case TokenType::MINUS:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) - std::any_cast<double>(right);
        case TokenType::PLUS:
            if (left.type() == typeid(double) && right.type() == typeid(double)) {
                return std::any_cast<double>(left) + std::any_cast<double>(right);
            }
            if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
                return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
            }
            throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");
        case TokenType::SLASH:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) / std::any_cast<double>(right);
        case TokenType::STAR:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) * std::any_cast<double>(right);
        default:
            // Unreachable.
            return {};
    }
}


bool Interpreter::isTruthy(const std::any& object) {
    if (object.type() == typeid(std::monostate)) return false;
    if (object.type() == typeid(bool)) return std::any_cast<bool>(object);
    return true;
}

bool Interpreter::isEqual(const std::any& a, const std::any& b) {
    if (a.type() == typeid(std::monostate) && b.type() == typeid(std::monostate)) return true;
    if (a.type() == typeid(std::monostate)) return false;

    if (a.type() != b.type()) return false;

    if (a.type() == typeid(std::string)) {
        return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
    }
    if (a.type() == typeid(double)) {
        return std::any_cast<double>(a) == std::any_cast<double>(b);
    }
    if (a.type() == typeid(bool)) {
        return std::any_cast<bool>(a) == std::any_cast<bool>(b);
    }

    return false;
}

void Interpreter::checkNumberOperand(const Token& op, const std::any& operand) {
    if (operand.type() == typeid(double)) return;
    throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(const Token& op, const std::any& left, const std::any& right) {
    if (left.type() == typeid(double) && right.type() == typeid(double)) return;
    throw RuntimeError(op, "Operands must be numbers.");
}

std::string Interpreter::stringify(const std::any& object) {
    if (object.type() == typeid(std::monostate)) return "nil";

    if (object.type() == typeid(double)) {
        std::string text = std::to_string(std::any_cast<double>(object));
        text.erase(text.find_last_not_of('0') + 1, std::string::npos);
        if (text.back() == '.') {
            text.pop_back();
        }
        return text;
    }

    if (object.type() == typeid(std::string)) {
        return std::any_cast<std::string>(object);
    }
    if (object.type() == typeid(bool)) {
        return std::any_cast<bool>(object) ? "true" : "false";
    }

    return "Error: unknown type in stringify";
}



std::any Interpreter::visitExpressionStmt(const Expression& stmt) {
    evaluate(*stmt.expression);
    return {};
}

std::any Interpreter::visitPrintStmt(const Print& stmt) {
    std::any value = evaluate(*stmt.expression);
    std::cout << stringify(value) << std::endl;
    return {};
}

std::any Interpreter::visitVarStmt(const Var& stmt) {
    std::any value = std::monostate{};
    if (stmt.initializer != nullptr) {
        value = evaluate(*stmt.initializer);
    }

    environment->define(stmt.name.lexeme, value);
    return {};
}
