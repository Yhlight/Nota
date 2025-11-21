#include "Interpreter.h"
#include <iostream>
#include <utility>

namespace nota {

class EnvironmentGuard {
public:
    EnvironmentGuard(std::shared_ptr<Environment>& current, std::shared_ptr<Environment> next)
        : current_(current), previous_(current) {
        current_ = std::move(next);
    }

    ~EnvironmentGuard() {
        current_ = previous_;
    }

private:
    std::shared_ptr<Environment>& current_;
    std::shared_ptr<Environment> previous_;
};

void Interpreter::interpret(const std::vector<std::shared_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        execute(statement);
    }
}

void Interpreter::visit(const std::shared_ptr<Block>& stmt) {
    for (const auto& statement : stmt->statements) {
        execute(statement);
    }
}

void Interpreter::visit(const std::shared_ptr<ExpressionStmt>& stmt) {
    evaluate(stmt->expression);
}

void Interpreter::visit(const std::shared_ptr<VarStmt>& stmt) {
    Value value = {};
    if (stmt->initializer) {
        value = evaluate(stmt->initializer);
    }
    environment_->define(stmt->name.lexeme, value);
}

void Interpreter::executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment) {
    EnvironmentGuard guard(this->environment_, std::move(environment));
    for (const auto& statement : statements) {
        execute(statement);
    }
}

void Interpreter::visit(const std::shared_ptr<IfStmt>& stmt) {
    if (isTruthy(evaluate(stmt->condition))) {
        execute(stmt->thenBranch);
    } else if (stmt->elseBranch) {
        execute(stmt->elseBranch);
    }
}

void Interpreter::visit(const std::shared_ptr<WhileStmt>& stmt) {
    while (isTruthy(evaluate(stmt->condition))) {
        execute(stmt->body);
    }
}

void Interpreter::visit(const std::shared_ptr<DoWhileStmt>& stmt) {
    do {
        execute(stmt->body);
    } while (isTruthy(evaluate(stmt->condition)));
}

void Interpreter::visit(const std::shared_ptr<CallExpr>& expr) {
    Value callee = evaluate(expr->callee);

    std::vector<Value> arguments;
    for (const auto& argument : expr->arguments) {
        arguments.push_back(evaluate(argument));
    }

    if (auto callable = std::get_if<std::shared_ptr<Callable>>(&callee)) {
        if (arguments.size() != (*callable)->arity()) {
            throw RuntimeError(expr->paren, "Expected " + std::to_string((*callable)->arity()) + " arguments but got " + std::to_string(arguments.size()) + ".");
        }
        lastValue_ = (*callable)->call(*this, arguments);
    } else {
        throw RuntimeError(expr->paren, "Can only call functions and classes.");
    }
}

void Interpreter::visit(const std::shared_ptr<FunctionStmt>& stmt) {
    auto function = std::make_shared<NotaFunction>(stmt, environment_);
    environment_->define(stmt->name.lexeme, function);
}

void Interpreter::visit(const std::shared_ptr<ReturnStmt>& stmt) {
    Value value = {};
    if (stmt->value != nullptr) {
        value = evaluate(stmt->value);
    }
    throw ReturnControl(value);
}

void Interpreter::visit(const std::shared_ptr<ClassStmt>& stmt) {
    std::map<std::string, std::shared_ptr<NotaFunction>> methods;
    for (const auto& method : stmt->methods) {
        bool isInitializer = method->name.lexeme == "init";
        auto function = std::make_shared<NotaFunction>(method, environment_, isInitializer);
        methods[method->name.lexeme] = function;
    }

    auto klass = std::make_shared<NotaClass>(stmt->name, methods);
    environment_->define(stmt->name.lexeme, klass);
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
    Value right = evaluate(expr->right);

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
}

void Interpreter::visit(const std::shared_ptr<Variable>& expr) {
    lastValue_ = environment_->get(expr->name);
}

void Interpreter::visit(const std::shared_ptr<Assign>& expr) {
    Value value = evaluate(expr->value);
    environment_->assign(expr->name, value);
    lastValue_ = value;
}

void Interpreter::visit(const std::shared_ptr<Postfix>& expr) {
    Value left = evaluate(expr->left);
    if (expr->op.type == TokenType::PLUS_PLUS) {
        if (std::holds_alternative<int>(left)) {
            lastValue_ = std::get<int>(left);
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
    if (auto instance = std::get_if<std::shared_ptr<NotaInstance>>(&object)) {
        lastValue_ = (*instance)->get(expr->name);
        return;
    }
    throw RuntimeError(expr->name, "Only instances have properties.");
}

void Interpreter::visit(const std::shared_ptr<SetExpr>& expr) {
    Value object = evaluate(expr->object);
    if (auto instance = std::get_if<std::shared_ptr<NotaInstance>>(&object)) {
        Value value = evaluate(expr->value);
        (*instance)->set(expr->name, value);
        lastValue_ = value;
        return;
    }
    throw RuntimeError(expr->name, "Only instances have fields.");
}

void Interpreter::visit(const std::shared_ptr<ThisExpr>& expr) {
    lastValue_ = environment_->get(expr->keyword);
}

void Interpreter::visit(const std::shared_ptr<Grouping>& expr) {
    lastValue_ = evaluate(expr->expression);
}

void Interpreter::visit(const std::shared_ptr<Literal>& expr) {
    if (std::holds_alternative<int>(expr->value)) {
        lastValue_ = std::get<int>(expr->value);
    } else if (std::holds_alternative<double>(expr->value)) {
        lastValue_ = std::get<double>(expr->value);
    } else if (std::holds_alternative<std::string>(expr->value)) {
        lastValue_ = std::get<std::string>(expr->value);
    } else if (std::holds_alternative<bool>(expr->value)) {
        lastValue_ = std::get<bool>(expr->value);
    } else {
        lastValue_ = {};
    }
}

void Interpreter::visit(const std::shared_ptr<Unary>& expr) {
    Value right = evaluate(expr->right);

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
    return true;
}

} // namespace nota
