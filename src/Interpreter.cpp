#include "Interpreter.h"
#include "NotaFunction.h"
#include "Return.h"
#include <chrono>
#include <stdexcept>

namespace nota {

class Clock : public Callable {
  public:
    int arity() override { return 0; }
    Value call(Interpreter &interpreter, std::vector<Value> arguments) override {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }
};

Interpreter::Interpreter() : globals(new Environment()), environment(globals) {
    globals->define("clock", new Clock());
}

void Interpreter::interpret(
    const std::vector<std::unique_ptr<ast::Stmt>> &statements) {
    for (const auto &statement : statements) {
        execute(*statement);
    }
}

Value Interpreter::evaluate(ast::Expr &expr) {
    return std::any_cast<Value>(expr.accept(*this));
}

void Interpreter::execute(ast::Stmt &stmt) { stmt.accept(*this); }

void Interpreter::execute_block(
    const std::vector<std::unique_ptr<ast::Stmt>> &statements,
    std::shared_ptr<Environment> environment) {
    auto previous = this->environment;
    this->environment = environment;
    for (const auto &statement : statements) {
        execute(*statement);
    }
    this->environment = previous;
}

bool Interpreter::is_truthy(const Value &value) {
    if (std::holds_alternative<std::monostate>(value))
        return false;
    if (std::holds_alternative<bool>(value))
        return std::get<bool>(value);
    return true;
}

std::any Interpreter::visit(ast::LiteralExpr &expr) {
    switch (expr.value.type) {
    case TokenType::Integer:
        return Value{std::stoll(expr.value.lexeme)};
    case TokenType::Float:
        return Value{std::stod(expr.value.lexeme)};
    case TokenType::String:
        return Value{expr.value.lexeme};
    case TokenType::True:
        return Value{true};
    case TokenType::False:
        return Value{false};
    default:
        return Value{std::monostate()};
    }
}

std::any Interpreter::visit(ast::UnaryExpr &expr) {
    Value right = evaluate(*expr.right);

    switch (expr.op.type) {
    case TokenType::Minus:
        if (std::holds_alternative<long long>(right)) {
            return Value{-std::get<long long>(right)};
        }
        if (std::holds_alternative<double>(right)) {
            return Value{-std::get<double>(right)};
        }
        break;
    case TokenType::Bang:
        return Value{!is_truthy(right)};
    default:
        break;
    }

    return Value{std::monostate()};
}

std::any Interpreter::visit(ast::BinaryExpr &expr) {
    Value left = evaluate(*expr.left);
    Value right = evaluate(*expr.right);

    switch (expr.op.type) {
    case TokenType::Plus:
        if (std::holds_alternative<long long>(left) &&
            std::holds_alternative<long long>(right)) {
            return Value{std::get<long long>(left) + std::get<long long>(right)};
        }
        if (std::holds_alternative<double>(left) &&
            std::holds_alternative<double>(right)) {
            return Value{std::get<double>(left) + std::get<double>(right)};
        }
        if (std::holds_alternative<std::string>(left) &&
            std::holds_alternative<std::string>(right)) {
            return Value{std::get<std::string>(left) +
                         std::get<std::string>(right)};
        }
        break;
    case TokenType::Minus:
        if (std::holds_alternative<long long>(left) &&
            std::holds_alternative<long long>(right)) {
            return Value{std::get<long long>(left) - std::get<long long>(right)};
        }
        if (std::holds_alternative<double>(left) &&
            std::holds_alternative<double>(right)) {
            return Value{std::get<double>(left) - std::get<double>(right)};
        }
        break;
    case TokenType::Star:
        if (std::holds_alternative<long long>(left) &&
            std::holds_alternative<long long>(right)) {
            return Value{std::get<long long>(left) * std::get<long long>(right)};
        }
        if (std::holds_alternative<double>(left) &&
            std::holds_alternative<double>(right)) {
            return Value{std::get<double>(left) * std::get<double>(right)};
        }
        break;
    case TokenType::Slash:
        if (std::holds_alternative<long long>(left) &&
            std::holds_alternative<long long>(right)) {
            return Value{std::get<long long>(left) / std::get<long long>(right)};
        }
        if (std::holds_alternative<double>(left) &&
            std::holds_alternative<double>(right)) {
            return Value{std::get<double>(left) / std::get<double>(right)};
        }
        break;
    case TokenType::Greater:
        if (std::holds_alternative<long long>(left) &&
            std::holds_alternative<long long>(right)) {
            return Value{std::get<long long>(left) > std::get<long long>(right)};
        }
        if (std::holds_alternative<double>(left) &&
            std::holds_alternative<double>(right)) {
            return Value{std::get<double>(left) > std::get<double>(right)};
        }
        break;
    case TokenType::GreaterEqual:
        if (std::holds_alternative<long long>(left) &&
            std::holds_alternative<long long>(right)) {
            return Value{std::get<long long>(left) >=
                         std::get<long long>(right)};
        }
        if (std::holds_alternative<double>(left) &&
            std::holds_alternative<double>(right)) {
            return Value{std::get<double>(left) >= std::get<double>(right)};
        }
        break;
    case TokenType::Less:
        if (std::holds_alternative<long long>(left) &&
            std::holds_alternative<long long>(right)) {
            return Value{std::get<long long>(left) < std::get<long long>(right)};
        }
        if (std::holds_alternative<double>(left) &&
            std::holds_alternative<double>(right)) {
            return Value{std::get<double>(left) < std::get<double>(right)};
        }
        break;
    case TokenType::LessEqual:
        if (std::holds_alternative<long long>(left) &&
            std::holds_alternative<long long>(right)) {
            return Value{std::get<long long>(left) <=
                         std::get<long long>(right)};
        }
        if (std::holds_alternative<double>(left) &&
            std::holds_alternative<double>(right)) {
            return Value{std::get<double>(left) <= std::get<double>(right)};
        }
        break;
    case TokenType::EqualEqual:
        return Value{left == right};
    case TokenType::BangEqual:
        return Value{left != right};
    default:
        break;
    }

    return Value{std::monostate()};
}

std::any Interpreter::visit(ast::VariableExpr &expr) {
    return environment->get(expr.name);
}

std::any Interpreter::visit(ast::AssignExpr &expr) {
    Value value = evaluate(*expr.value);
    environment->assign(expr.name, value);
    return value;
}

std::any Interpreter::visit(ast::PostfixExpr &expr) {
    return Value{std::monostate()};
}

std::any Interpreter::visit(ast::CallExpr &expr) {
    Value callee = evaluate(*expr.callee);

    std::vector<Value> arguments;
    for (const auto &arg : expr.arguments) {
        arguments.push_back(evaluate(*arg));
    }

    if (std::holds_alternative<Callable *>(callee)) {
        auto *function = std::get<Callable *>(callee);
        if (arguments.size() != function->arity()) {
            throw std::runtime_error("Expected " +
                                     std::to_string(function->arity()) +
                                     " arguments but got " +
                                     std::to_string(arguments.size()) + ".");
        }
        return function->call(*this, arguments);
    } else {
        throw std::runtime_error("Can only call functions and classes.");
    }
}

std::any Interpreter::visit(ast::LambdaExpr &expr) {
    return Value{std::monostate()};
}

std::any Interpreter::visit(ast::VarDeclStmt &stmt) {
    Value value = std::monostate();
    if (stmt.initializer) {
        value = evaluate(*stmt.initializer);
    }
    environment->define(stmt.name.lexeme, value);
    return {};
}

std::any Interpreter::visit(ast::BlockStmt &stmt) {
    execute_block(stmt.statements, std::make_shared<Environment>(environment));
    return {};
}

std::any Interpreter::visit(ast::IfStmt &stmt) {
    if (is_truthy(evaluate(*stmt.condition))) {
        execute(*stmt.then_branch);
    } else if (stmt.else_branch) {
        execute(*stmt.else_branch);
    }
    return {};
}

std::any Interpreter::visit(ast::WhileStmt &stmt) {
    while (is_truthy(evaluate(*stmt.condition))) {
        execute(*stmt.body);
    }
    return {};
}

std::any Interpreter::visit(ast::DoWhileStmt &stmt) {
    do {
        execute(*stmt.body);
    } while (is_truthy(evaluate(*stmt.condition)));
    return {};
}

std::any Interpreter::visit(ast::ExpressionStmt &stmt) {
    evaluate(*stmt.expression);
    return {};
}

std::any Interpreter::visit(ast::ForEachStmt &stmt) {
    return {};
}

std::any Interpreter::visit(ast::ForStmt &stmt) {
    return {};
}

std::any Interpreter::visit(ast::MatchStmt &stmt) {
    return {};
}

std::any Interpreter::visit(ast::FuncDeclStmt &stmt) {
    auto function = new NotaFunction(&stmt, environment);
    environment->define(stmt.name.lexeme, function);
    return {};
}

std::any Interpreter::visit(ast::ReturnStmt &stmt) {
    Value value = std::monostate();
    if (stmt.value) {
        value = evaluate(*stmt.value);
    }
    throw Return(value);
}

} // namespace nota
