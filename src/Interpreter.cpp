#include "Interpreter.h"
#include "NotaFunction.h"
#include "NotaLambda.h"
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

Interpreter::Interpreter(ModuleManager &module_manager) : module_manager(module_manager), globals(new Environment()), environment(globals) {
    globals->define("clock", new Clock());
}

void Interpreter::interpret(
    const std::vector<std::unique_ptr<ast::Stmt>> &statements, const std::string &path) {
    this->path = path;
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

Value Interpreter::evaluate_in_environment(ast::Expr &expr, std::shared_ptr<Environment> environment) {
    auto previous = this->environment;
    this->environment = environment;
    Value result = evaluate(expr);
    this->environment = previous;
    return result;
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
    if (ast::VariableExpr* var = dynamic_cast<ast::VariableExpr*>(expr.left.get())) {
        Value value = environment->get(var->name);
        if (std::holds_alternative<long long>(value)) {
            long long current_value = std::get<long long>(value);
            long long next_value = current_value;
            if (expr.op.type == TokenType::PlusPlus) {
                next_value++;
            } else if (expr.op.type == TokenType::MinusMinus) {
                next_value--;
            }
            environment->assign(var->name, next_value);
            return Value{current_value};
        } else if (std::holds_alternative<double>(value)) {
            double current_value = std::get<double>(value);
            double next_value = current_value;
            if (expr.op.type == TokenType::PlusPlus) {
                next_value++;
            } else if (expr.op.type == TokenType::MinusMinus) {
                next_value--;
            }
            environment->assign(var->name, next_value);
            return Value{current_value};
        }
    }
    throw std::runtime_error("Operand of postfix operator must be a variable.");
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
    return Value{new NotaLambda(&expr, environment)};
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

    if (std::holds_alternative<std::shared_ptr<Environment>>(object)) {
        return std::get<std::shared_ptr<Environment>>(object)->get(expr.name);
    }

    throw std::runtime_error("Only instances and modules have properties.");
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

    if (stmt.type) {
        if (ast::ArrayType* array_type = dynamic_cast<ast::ArrayType*>(stmt.type.get())) {
            if (array_type->size) {
                Value size_val = evaluate(*array_type->size);
                if (std::holds_alternative<long long>(size_val)) {
                    long long size = std::get<long long>(size_val);
                    if (std::holds_alternative<NotaArray *>(value)) {
                        if (std::get<NotaArray *>(value)->size() > size) {
                            throw std::runtime_error("Array initializer too large for static array.");
                        }
                    }
                    std::vector<Value> elements(size, std::monostate());
                    if (std::holds_alternative<NotaArray *>(value)) {
                        for(int i = 0; i < std::get<NotaArray *>(value)->size(); ++i) {
                            elements[i] = std::get<NotaArray *>(value)->get(i);
                        }
                    }
                    value = new NotaArray(elements);
                } else {
                    throw std::runtime_error("Array size must be an integer.");
                }
            }
        }
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
    Value container = evaluate(*stmt.container);
    if (std::holds_alternative<NotaArray *>(container)) {
        NotaArray *array = std::get<NotaArray *>(container);
        for (int i = 0; i < array->size(); ++i) {
            auto loop_env = std::make_shared<Environment>(environment);
            auto previous = this->environment;
            this->environment = loop_env;
            environment->define(stmt.variable.lexeme, array->get(i));
            execute(*stmt.body);
            this->environment = previous;
        }
    } else {
        throw std::runtime_error("For-each loop requires an array container.");
    }
    return {};
}

std::any Interpreter::visit(ast::ForStmt &stmt) {
    auto loop_env = std::make_shared<Environment>(environment);
    auto previous = this->environment;
    this->environment = loop_env;

    if (stmt.initializer) {
        execute(*stmt.initializer);
    }

    while (true) {
        if (stmt.condition) {
            if (!is_truthy(evaluate(*stmt.condition))) {
                break;
            }
        }
        execute(*stmt.body);
        if (stmt.increment) {
            evaluate(*stmt.increment);
        }
    }

    this->environment = previous;
    return {};
}

std::any Interpreter::visit(ast::MatchStmt &stmt) {
    Value value = evaluate(*stmt.expression);
    for (auto &match_case : stmt.cases) {
        for (auto &case_value_expr : match_case.values) {
            Value case_value = evaluate(*case_value_expr);
            if (value == case_value) {
                execute(*match_case.body);
                return {};
            }
        }
    }

    if (stmt.else_branch) {
        execute(*stmt.else_branch);
    }

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

std::string get_path(ast::Expr *expr) {
    if (auto *literal = dynamic_cast<ast::LiteralExpr *>(expr)) {
        return literal->value.lexeme.substr(1, literal->value.lexeme.length() - 2);
    }
    if (auto *get = dynamic_cast<ast::GetExpr *>(expr)) {
        return get_path(get->object.get()) + "::" + get->name.lexeme;
    }
    if (auto *variable = dynamic_cast<ast::VariableExpr *>(expr)) {
        return variable->name.lexeme;
    }
    throw std::runtime_error("Invalid import path.");
}

std::any Interpreter::visit(ast::ImportStmt &stmt) {
    std::string path = get_path(stmt.path.get());
    const auto &stmts = module_manager.resolve_import(path);

    auto module_env = std::make_shared<Environment>();
    execute_block(stmts, module_env);

    std::string name;
    if (stmt.alias) {
        name = stmt.alias->lexeme;
    } else {
        size_t double_colon_pos = path.find("::");
        if (double_colon_pos != std::string::npos) {
            name = path.substr(double_colon_pos + 2);
        } else {
            size_t last_slash_pos = path.find_last_of('/');
            if (last_slash_pos != std::string::npos) {
                name = path.substr(last_slash_pos + 1);
            } else {
                name = path;
            }
        }
        size_t last_dot_pos = name.rfind(".nota");
        if (last_dot_pos != std::string::npos) {
            name = name.substr(0, last_dot_pos);
        }
    }
    environment->define(name, module_env);

    return {};
}

std::any Interpreter::visit(ast::PackageDeclStmt &stmt) {
    std::string dir;
    size_t last_slash = path.find_last_of("/\\");
    if (last_slash != std::string::npos) {
        dir = path.substr(0, last_slash);
    } else {
        dir = ".";
    }
    module_manager.register_package(stmt.name.lexeme, dir);
    return {};
}

} // namespace nota
