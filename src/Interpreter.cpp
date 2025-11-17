#include "Interpreter.h"
#include "NotaFunction.h"
#include "NotaArray.h"
#include "NotaClass.h"
#include "NotaInstance.h"
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
    if (ast::GetExpr *get = dynamic_cast<ast::GetExpr *>(expr.name.get())) {
        Value object = evaluate(*get->object);
        if (std::holds_alternative<NotaInstance *>(object)) {
            std::get<NotaInstance *>(object)->set(get->name, value);
            return value;
        }
        throw std::runtime_error("Only instances have fields.");
    } else if (ast::SubscriptExpr *subscript = dynamic_cast<ast::SubscriptExpr *>(expr.name.get())) {
        Value callee = evaluate(*subscript->callee);
        Value index = evaluate(*subscript->index);

        if (std::holds_alternative<NotaArray *>(callee)) {
            if (std::holds_alternative<long long>(index)) {
                std::get<NotaArray *>(callee)->set(std::get<long long>(index), value);
                return value;
            } else {
                throw std::runtime_error("Array index must be an integer.");
            }
        } else {
            throw std::runtime_error("Can only subscript arrays.");
        }
    } else if (ast::VariableExpr *var = dynamic_cast<ast::VariableExpr *>(expr.name.get())) {
        environment->assign(var->name, value);
        return value;
    }

    throw std::runtime_error("Invalid assignment target.");
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
    } else if(std::holds_alternative<NotaClass *>(callee)) {
        auto *klass = std::get<NotaClass *>(callee);
        if (arguments.size() != klass->arity()) {
            throw std::runtime_error("Expected " +
                                     std::to_string(klass->arity()) +
                                     " arguments but got " +
                                     std::to_string(arguments.size()) + ".");
        }
        return klass->call(*this, arguments);
    }else {
        throw std::runtime_error("Can only call functions and classes.");
    }
}

std::any Interpreter::visit(ast::LambdaExpr &expr) {
    return Value{std::monostate()};
}

std::any Interpreter::visit(ast::ArrayLiteralExpr &expr) {
    std::vector<Value> elements;
    for (const auto &element : expr.elements) {
        elements.push_back(evaluate(*element));
    }
    return Value{new NotaArray(elements)};
}

std::any Interpreter::visit(ast::SubscriptExpr &expr) {
    Value callee = evaluate(*expr.callee);
    Value index = evaluate(*expr.index);

    if (std::holds_alternative<NotaArray *>(callee)) {
        if (std::holds_alternative<long long>(index)) {
            return Value{std::get<NotaArray *>(callee)->get(std::get<long long>(index))};
        } else {
            throw std::runtime_error("Array index must be an integer.");
        }
    } else {
        throw std::runtime_error("Can only subscript arrays.");
    }
}

std::any Interpreter::visit(ast::GetExpr &expr) {
    Value object = evaluate(*expr.object);
    if (std::holds_alternative<NotaInstance *>(object)) {
        return std::get<NotaInstance *>(object)->get(expr.name);
    }

    throw std::runtime_error("Only instances have properties.");
}

std::any Interpreter::visit(ast::SetExpr &expr) {
    Value object = evaluate(*expr.object);
    if (std::holds_alternative<NotaInstance *>(object)) {
        Value value = evaluate(*expr.value);
        std::get<NotaInstance *>(object)->set(expr.name, value);
        return value;
    }

    throw std::runtime_error("Only instances have fields.");
}

std::any Interpreter::visit(ast::ThisExpr &expr) {
    return environment->get(expr.keyword);
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
    auto function = new NotaFunction(&stmt, environment, false);
    environment->define(stmt.name.lexeme, function);
    return {};
}

std::any Interpreter::visit(ast::ClassDeclStmt &stmt) {
    environment->define(stmt.name.lexeme, std::monostate());

    std::map<std::string, NotaFunction *> methods;
    for (const auto &method : stmt.methods) {
        auto function = new NotaFunction(method.get(), environment, method->name.lexeme == "new");
        methods[method->name.lexeme] = function;
    }

    auto klass = new NotaClass(stmt.name.lexeme, methods);
    environment->assign(stmt.name, klass);
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
