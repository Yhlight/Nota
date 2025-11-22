#include "Interpreter.h"
#include "ModuleLoader.h"
#include "StdLib.h"
#include <iostream>
#include <utility>

namespace nota {

class EnvironmentGuard {
public:
    EnvironmentGuard(Environment*& current, Environment* next)
        : current_(current), previous_(current) {
        current_ = next;
    }

    ~EnvironmentGuard() {
        current_ = previous_;
    }

private:
    Environment*& current_;
    Environment* previous_;
};

Interpreter::Interpreter(VM& vm) : vm(vm), environment_(nullptr), moduleLoader_(std::make_unique<ModuleLoader>(*this)) {
    environment_ = vm.newObject<Environment>();
    vm.setInterpreter(this);

    registerNative("print", -1, stdlib::print);
    registerNative("clock", 0, stdlib::clock);
}

void Interpreter::registerNative(const std::string& name, int arity, NotaNativeFunction::NativeFn fn) {
    auto native_fn = vm.newObject<NotaNativeFunction>(arity, std::move(fn));
    environment_->define(name, native_fn);
}

Interpreter::~Interpreter() {
    vm.setInterpreter(nullptr);
}

void Interpreter::interpret(const std::vector<std::shared_ptr<Stmt>>& statements) {
    statements_ = statements;
    for (const auto& statement : statements_) {
        execute(statement);
    }
}

void Interpreter::markRoots() {
    vm.markObject(environment_);
    for (auto const& [key, val] : modules_) {
        val->markRoots();
    }
    for (auto const& val : stack_) {
        vm.markValue(val);
    }
}

void Interpreter::visit(const std::shared_ptr<Block>& stmt) {
    for (const auto& statement : stmt->statements) {
        execute(statement);
    }
}

void Interpreter::visit(const std::shared_ptr<ExpressionStmt>& stmt) {
    evaluate(stmt->expression);
    stack_.pop_back();
}

void Interpreter::visit(const std::shared_ptr<VarStmt>& stmt) {
    Value value = {};
    if (stmt->initializer) {
        value = evaluate(stmt->initializer);
        stack_.pop_back();
    }
    environment_->define(stmt->name.lexeme, value);
}

void Interpreter::executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, Environment* environment) {
    EnvironmentGuard guard(this->environment_, environment);
    for (const auto& statement : statements) {
        execute(statement);
    }
}

void Interpreter::visit(const std::shared_ptr<IfStmt>& stmt) {
    Value condition = evaluate(stmt->condition);
    stack_.pop_back();
    if (isTruthy(condition)) {
        execute(stmt->thenBranch);
    } else if (stmt->elseBranch) {
        execute(stmt->elseBranch);
    }
}

void Interpreter::visit(const std::shared_ptr<WhileStmt>& stmt) {
    while (true) {
        Value condition = evaluate(stmt->condition);
        stack_.pop_back();
        if (!isTruthy(condition)) {
            break;
        }
        execute(stmt->body);
    }
}

void Interpreter::visit(const std::shared_ptr<DoWhileStmt>& stmt) {
    Environment* loopEnvironment = vm.newObject<Environment>(environment_);
    do {
        {
            EnvironmentGuard guard(this->environment_, loopEnvironment);
            execute(stmt->body);
        }

        Value condition = evaluate(stmt->condition);
        stack_.pop_back();
        if (!isTruthy(condition)) {
            break;
        }
    } while (true);
}

void Interpreter::visit(const std::shared_ptr<CallExpr>& expr) {
    Value callee = evaluate(expr->callee);
    stack_.pop_back();

    std::vector<Value> arguments;
    for (const auto& argument : expr->arguments) {
        arguments.push_back(evaluate(argument));
        stack_.pop_back();
    }

    if (auto callable = std::get_if<Object*>(&callee)) {
        if (auto c = dynamic_cast<Callable*>(*callable)) {
            if (c->arity() != -1 && arguments.size() != c->arity()) {
                throw RuntimeError(expr->paren, "Expected " + std::to_string(c->arity()) + " arguments but got " + std::to_string(arguments.size()) + ".");
            }
            lastValue_ = c->call(*this, arguments);
            stack_.push_back(lastValue_);
        } else {
            throw RuntimeError(expr->paren, "Can only call functions and classes.");
        }
    } else {
        throw RuntimeError(expr->paren, "Can only call functions and classes.");
    }
}

void Interpreter::visit(const std::shared_ptr<FunctionStmt>& stmt) {
    auto function = vm.newObject<NotaFunction>(stmt, environment_);
    environment_->define(stmt->name.lexeme, function);
}

void Interpreter::visit(const std::shared_ptr<ReturnStmt>& stmt) {
    Value value = {};
    if (stmt->value != nullptr) {
        value = evaluate(stmt->value);
        stack_.pop_back();
    }
    throw ReturnControl(value);
}

void Interpreter::visit(const std::shared_ptr<ClassStmt>& stmt) {
    std::map<std::string, NotaFunction*> methods;
    for (const auto& method : stmt->methods) {
        bool isInitializer = method->name.lexeme == "init";
        auto function = vm.newObject<NotaFunction>(method, environment_, isInitializer);
        methods[method->name.lexeme] = function;
    }

    auto klass = vm.newObject<NotaClass>(stmt->name, methods);
    environment_->define(stmt->name.lexeme, klass);
}

void Interpreter::visit(const std::shared_ptr<ImportStmt>& stmt) {
    std::string path = std::get<std::string>(stmt->path.literal);
    std::string name;
    if (stmt->alias) {
        name = stmt->alias->lexeme;
    } else {
        size_t last_slash = path.find_last_of("/\\");
        name = (last_slash == std::string::npos) ? path : path.substr(last_slash + 1);
        size_t last_dot = name.find_last_of('.');
        if (last_dot != std::string::npos) {
            name = name.substr(0, last_dot);
        }
    }
    modules_[name] = moduleLoader_->load(path, vm);
}

void Interpreter::visit(const std::shared_ptr<PackageStmt>& stmt) {
    // Not implemented yet
}

namespace {
    void checkNumberOperands(const Token& op, const Value& left, const Value& right) {
        if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) return;
        if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) return;
        throw Interpreter::RuntimeError(op, "Operands must be two numbers.");
    }
}
void Interpreter::visit(const std::shared_ptr<Binary>& expr) {
    Value left = evaluate(expr->left);
    stack_.pop_back();
    Value right = evaluate(expr->right);
    stack_.pop_back();

    switch (expr->op.type) {
        case TokenType::PLUS:
             if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                lastValue_ = std::get<int>(left) + std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                lastValue_ = std::get<double>(left) + std::get<double>(right);
            } else {
                throw RuntimeError(expr->op, "Operands must be two numbers.");
            }
            break;
        case TokenType::EQUALS:
            lastValue_ = left == right;
            break;
        case TokenType::NOT_EQUALS:
            lastValue_ = left != right;
            break;
        case TokenType::LESS:
            checkNumberOperands(expr->op, left, right);
             if (std::holds_alternative<int>(left)) {
                lastValue_ = std::get<int>(left) < std::get<int>(right);
            } else {
                lastValue_ = std::get<double>(left) < std::get<double>(right);
            }
            break;
        case TokenType::GREATER:
            checkNumberOperands(expr->op, left, right);
            if (std::holds_alternative<int>(left)) {
                lastValue_ = std::get<int>(left) > std::get<int>(right);
            } else {
                lastValue_ = std::get<double>(left) > std::get<double>(right);
            }
            break;
        case TokenType::LESS_EQUALS:
            checkNumberOperands(expr->op, left, right);
            if (std::holds_alternative<int>(left)) {
                lastValue_ = std::get<int>(left) <= std::get<int>(right);
            } else {
                lastValue_ = std::get<double>(left) <= std::get<double>(right);
            }
            break;
        case TokenType::GREATER_EQUALS:
            checkNumberOperands(expr->op, left, right);
            if (std::holds_alternative<int>(left)) {
                lastValue_ = std::get<int>(left) >= std::get<int>(right);
            } else {
                lastValue_ = std::get<double>(left) >= std::get<double>(right);
            }
            break;
        default:
            lastValue_ = {};
            break;
    }
    stack_.push_back(lastValue_);
}

void Interpreter::visit(const std::shared_ptr<Variable>& expr) {
    lastValue_ = environment_->get(expr->name);
    stack_.push_back(lastValue_);
}

void Interpreter::visit(const std::shared_ptr<Assign>& expr) {
    Value value = evaluate(expr->value);
    stack_.pop_back();
    environment_->assign(expr->name, value);
    lastValue_ = value;
    stack_.push_back(lastValue_);
}

void Interpreter::visit(const std::shared_ptr<Postfix>& expr) {
    Value left = evaluate(expr->left);
    stack_.pop_back();
    if (expr->op.type == TokenType::PLUS_PLUS) {
        if (std::holds_alternative<int>(left)) {
            lastValue_ = std::get<int>(left);
            stack_.push_back(lastValue_);
            if (auto var = std::dynamic_pointer_cast<Variable>(expr->left)) {
                environment_->assign(var->name, std::get<int>(left) + 1);
            } else {
                throw RuntimeError(expr->op, "Operand of increment must be a variable.");
            }
        } else {
            throw RuntimeError(expr->op, "Operand of increment must be a number.");
        }
    }
}

void Interpreter::visit(const std::shared_ptr<GetExpr>& expr) {
    Value object = evaluate(expr->object);
    stack_.pop_back();
    if (auto obj = std::get_if<Object*>(&object)) {
        if (auto instance = dynamic_cast<NotaInstance*>(*obj)) {
            lastValue_ = instance->get(*this, expr->name);
            stack_.push_back(lastValue_);
            return;
        }
    }
    throw RuntimeError(expr->name, "Only instances have properties.");
}

void Interpreter::visit(const std::shared_ptr<SetExpr>& expr) {
    Value object = evaluate(expr->object);
    stack_.pop_back();
    if (auto obj = std::get_if<Object*>(&object)) {
        if (auto instance = dynamic_cast<NotaInstance*>(*obj)) {
            Value value = evaluate(expr->value);
            stack_.pop_back();
            instance->set(expr->name, value);
            lastValue_ = value;
            stack_.push_back(lastValue_);
            return;
        }
    }
    throw RuntimeError(expr->name, "Only instances have fields.");
}

void Interpreter::visit(const std::shared_ptr<ThisExpr>& expr) {
    lastValue_ = environment_->get(expr->keyword);
    stack_.push_back(lastValue_);
}

void Interpreter::visit(const std::shared_ptr<ModuleAccessExpr>& expr) {
    auto it = modules_.find(expr->module.lexeme);
    if (it == modules_.end()) {
        throw RuntimeError(expr->module, "Module not found: " + expr->module.lexeme);
    }
    lastValue_ = it->second->getEnvironment()->get(expr->member);
    stack_.push_back(lastValue_);
}

void Interpreter::visit(const std::shared_ptr<LambdaExpr>& expr) {
    // Create a synthetic FunctionStmt to represent the lambda
    Token name{TokenType::IDENTIFIER, "lambda", {}, -1};
    auto functionStmt = std::make_shared<FunctionStmt>(name, expr->params, expr->body);

    auto function = vm.newObject<NotaFunction>(functionStmt, environment_);
    lastValue_ = function;
    stack_.push_back(lastValue_);
}

void Interpreter::visit(const std::shared_ptr<Grouping>& expr) {
    lastValue_ = evaluate(expr->expression);
    stack_.pop_back();
    stack_.push_back(lastValue_);
}

void Interpreter::visit(const std::shared_ptr<Literal>& expr) {
    if (std::holds_alternative<int>(expr->value)) {
        lastValue_ = std::get<int>(expr->value);
    } else if (std::holds_alternative<double>(expr->value)) {
        lastValue_ = std::get<double>(expr->value);
    } else if (std::holds_alternative<std::string>(expr->value)) {
        lastValue_ = vm.newObject<NotaString>(std::get<std::string>(expr->value));
    } else if (std::holds_alternative<bool>(expr->value)) {
        lastValue_ = std::get<bool>(expr->value);
    } else {
        lastValue_ = {};
    }
    stack_.push_back(lastValue_);
}

void Interpreter::visit(const std::shared_ptr<Unary>& expr) {
    Value right = evaluate(expr->right);
    stack_.pop_back();

    switch (expr->op.type) {
        case TokenType::MINUS:
            if (std::holds_alternative<int>(right)) {
                lastValue_ = -std::get<int>(right);
            } else if (std::holds_alternative<double>(right)) {
                lastValue_ = -std::get<double>(right);
            } else {
                throw RuntimeError(expr->op, "Operand must be a number.");
            }
            break;
        case TokenType::NOT:
            lastValue_ = !isTruthy(right);
            break;
        default:
            lastValue_ = {};
            break;
    }
    stack_.push_back(lastValue_);
}

void Interpreter::execute(const std::shared_ptr<Stmt>& stmt) {
    stmt->accept(*this);
}

Value Interpreter::evaluate(const std::shared_ptr<Expr>& expr) {
    expr->accept(*this);
    return lastValue_;
}

bool Interpreter::isTruthy(const Value& value) {
    if (std::holds_alternative<std::monostate>(value)) return false;
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
    if (std::holds_alternative<double>(value)) return std::get<double>(value) != 0.0;
    if (std::holds_alternative<int>(value)) return std::get<int>(value) != 0;
    if (auto obj = std::get_if<Object*>(&value)) {
        if (*obj == nullptr) return false;
    }
    return true;
}

} // namespace nota
