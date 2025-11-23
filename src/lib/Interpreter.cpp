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
    // Native functions are immutable
    environment_->define(name, native_fn, false, nullptr);
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

    if (stmt->type) {
        if (stmt->type->is_array && stmt->type->size) {
            if (auto* obj = std::get_if<Object*>(&value)) {
                if ((*obj)->type == ObjectType::ARRAY) {
                    auto* array = static_cast<NotaArray*>(*obj);
                    Value size_val = evaluate(stmt->type->size);
                    if (std::holds_alternative<int>(size_val)) {
                        array->capacity = std::get<int>(size_val);
                        if (array->elements.size() > array->capacity) {
                            throw RuntimeError(stmt->type->name, "Array size exceeds static capacity.");
                        }
                    } else {
                        throw RuntimeError(stmt->type->name, "Array capacity must be an integer.");
                    }
                }
            }
        }
        checkType(value, stmt->type, true); // Allow implicit conversion
    }

    environment_->define(stmt->name.lexeme, value, stmt->is_mutable, stmt);
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
    EnvironmentGuard guard(this->environment_, loopEnvironment);
    do {
        execute(stmt->body);

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
    environment_->define(stmt->name.lexeme, function, false, nullptr);
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
    std::map<std::string, NotaFunction*> static_methods;
    for (const auto& method : stmt->methods) {
        bool isInitializer = method->name.lexeme == "init";
        auto function = vm.newObject<NotaFunction>(method, environment_, isInitializer);
        if (method->is_static) {
            static_methods[method->name.lexeme] = function;
        } else {
            methods[method->name.lexeme] = function;
        }
    }

    auto klass = vm.newObject<NotaClass>(stmt->name, stmt->properties, methods, static_methods);
    environment_->define(stmt->name.lexeme, klass, false, nullptr);
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
    packageName_ = stmt->name.lexeme;
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
            } else if (std::holds_alternative<Object*>(left) && std::holds_alternative<Object*>(right)) {
                auto* left_obj = std::get<Object*>(left);
                auto* right_obj = std::get<Object*>(right);
                if (left_obj && right_obj && left_obj->type == ObjectType::STRING && right_obj->type == ObjectType::STRING) {
                    auto* left_str = static_cast<NotaString*>(left_obj);
                    auto* right_str = static_cast<NotaString*>(right_obj);
                    lastValue_ = vm.newObject<NotaString>(left_str->value + right_str->value);
                } else {
                    throw RuntimeError(expr->op, "Operands must be two numbers or two strings.");
                }
            } else {
                throw RuntimeError(expr->op, "Operands must be two numbers or two strings.");
            }
            break;
        case TokenType::MINUS:
            checkNumberOperands(expr->op, left, right);
            if (std::holds_alternative<int>(left)) {
                lastValue_ = std::get<int>(left) - std::get<int>(right);
            } else {
                lastValue_ = std::get<double>(left) - std::get<double>(right);
            }
            break;
        case TokenType::STAR:
            checkNumberOperands(expr->op, left, right);
            if (std::holds_alternative<int>(left)) {
                lastValue_ = std::get<int>(left) * std::get<int>(right);
            } else {
                lastValue_ = std::get<double>(left) * std::get<double>(right);
            }
            break;
        case TokenType::SLASH:
            checkNumberOperands(expr->op, left, right);
            if (std::holds_alternative<int>(left)) {
                lastValue_ = std::get<int>(left) / std::get<int>(right);
            } else {
                lastValue_ = std::get<double>(left) / std::get<double>(right);
            }
            break;
        case TokenType::PERCENT:
            checkNumberOperands(expr->op, left, right);
            if (std::holds_alternative<int>(left)) {
                lastValue_ = std::get<int>(left) % std::get<int>(right);
            } else {
                throw RuntimeError(expr->op, "Operands of '%' must be integers.");
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
        case TokenType::AMPERSAND:
            checkNumberOperands(expr->op, left, right);
            lastValue_ = std::get<int>(left) & std::get<int>(right);
            break;
        case TokenType::PIPE:
            checkNumberOperands(expr->op, left, right);
            lastValue_ = std::get<int>(left) | std::get<int>(right);
            break;
        case TokenType::CARET:
            checkNumberOperands(expr->op, left, right);
            lastValue_ = std::get<int>(left) ^ std::get<int>(right);
            break;
        case TokenType::LEFT_SHIFT:
            checkNumberOperands(expr->op, left, right);
            lastValue_ = std::get<int>(left) << std::get<int>(right);
            break;
        case TokenType::RIGHT_SHIFT:
            checkNumberOperands(expr->op, left, right);
            lastValue_ = std::get<int>(left) >> std::get<int>(right);
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

    if (auto token = std::get_if<Token>(&expr->accessor)) {
        if (auto obj = std::get_if<Object*>(&object)) {
            if (auto instance = dynamic_cast<NotaInstance*>(*obj)) {
                lastValue_ = instance->get(*this, *token);
                stack_.push_back(lastValue_);
                return;
            }
        }
        throw RuntimeError(*token, "Only instances have properties.");
    }

    throw std::runtime_error("Invalid accessor type in GetExpr.");
}

void Interpreter::visit(const std::shared_ptr<SetExpr>& expr) {
    Value object = evaluate(expr->object);
    stack_.pop_back();
    Value value = evaluate(expr->value);
    stack_.pop_back();

    if (auto token = std::get_if<Token>(&expr->accessor)) {
        if (auto obj = std::get_if<Object*>(&object)) {
            if (auto instance = dynamic_cast<NotaInstance*>(*obj)) {
                auto* klass = instance->getClass();
                if (auto prop_type = klass->findProperty(token->lexeme)) {
                    checkType(value, prop_type, true);
                }
                instance->set(*token, value);
                lastValue_ = value;
                stack_.push_back(lastValue_);
                return;
            }
        }
        throw RuntimeError(*token, "Only instances have fields.");
    } else if (auto index_expr = std::get_if<std::shared_ptr<Expr>>(&expr->accessor)) {
        Value index_val = evaluate(*index_expr);
        stack_.pop_back();
        if (auto obj = std::get_if<Object*>(&object)) {
            if ((*obj)->type == ObjectType::ARRAY) {
                if(auto var = std::dynamic_pointer_cast<Variable>(expr->object)){
                    if(auto var_stmt = environment_->findVariable(var->name)){
                        if(var_stmt->type && var_stmt->type->is_array){
                            auto element_type = std::make_shared<TypeExpr>(var_stmt->type->name);
                            checkType(value, element_type, true);
                        }
                    }
                }
                auto array = static_cast<NotaArray*>(*obj);
                if (std::holds_alternative<int>(index_val)) {
                    int index = std::get<int>(index_val);
                     if (array->capacity != -1 && index >= array->capacity) {
                        throw RuntimeError(expr->token_for_error, "Array index out of bounds for static array.");
                    }
                    if (index >= 0 && index < array->elements.size()) {
                        array->elements[index] = value;
                        lastValue_ = value;
                        stack_.push_back(lastValue_);
                        return;
                    } else if (index >= 0 && index < array->capacity) {
                        // Grow the array to fit the new element
                        array->elements.resize(index + 1);
                        array->elements[index] = value;
                        lastValue_ = value;
                        stack_.push_back(lastValue_);
                        return;
                    }
                }
                throw RuntimeError(expr->token_for_error, "Array index must be a non-negative integer.");
            } else if ((*obj)->type == ObjectType::STRING) {
                throw RuntimeError(expr->token_for_error, "Strings are immutable.");
            }
        }
        throw RuntimeError(expr->token_for_error, "Can only subscript arrays.");
    }
}

void Interpreter::visit(const std::shared_ptr<ThisExpr>& expr) {
    lastValue_ = environment_->get(expr->keyword);
    stack_.push_back(lastValue_);
}

void Interpreter::visit(const std::shared_ptr<ScopeAccessExpr>& expr) {
    // Check if the scope is a class variable
    if (environment_->isDefined(expr->scope.lexeme)) {
        Value scope = environment_->get(expr->scope);
        if (auto* obj = std::get_if<Object*>(&scope)) {
            if (auto* klass = dynamic_cast<NotaClass*>(*obj)) {
                if (auto* method = klass->findStaticMethod(expr->member.lexeme)) {
                    lastValue_ = method;
                    stack_.push_back(lastValue_);
                    return;
                }
                throw RuntimeError(expr->member, "Undefined static method '" + expr->member.lexeme + "'.");
            }
        }
    }

    // Check if the scope is a module
    auto it = modules_.find(expr->scope.lexeme);
    if (it != modules_.end()) {
        lastValue_ = it->second->getEnvironment()->get(expr->member);
        stack_.push_back(lastValue_);
        return;
    }

    throw RuntimeError(expr->scope, "Undefined variable or module '" + expr->scope.lexeme + "'.");
}

void Interpreter::visit(const std::shared_ptr<LambdaExpr>& expr) {
    // Create a synthetic FunctionStmt to represent the lambda
    Token name{TokenType::IDENTIFIER, "lambda", {}, -1};
    auto functionStmt = std::make_shared<FunctionStmt>(name, expr->params, expr->body, expr->return_type, false);

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
        case TokenType::TILDE:
            if (std::holds_alternative<int>(right)) {
                lastValue_ = ~std::get<int>(right);
            } else {
                throw RuntimeError(expr->op, "Operand must be an integer.");
            }
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

void Interpreter::visit(const std::shared_ptr<ArrayExpr>& expr) {
    std::vector<Value> elements;
    for (const auto& element : expr->elements) {
        elements.push_back(evaluate(element));
        stack_.pop_back();
    }

    lastValue_ = vm.newObject<NotaArray>(elements);
    stack_.push_back(lastValue_);
}

void Interpreter::visit(const std::shared_ptr<SubscriptExpr>& expr) {
    Value object = evaluate(expr->object);
    stack_.pop_back();
    Value index_val = evaluate(expr->index);
    stack_.pop_back();

    if (auto obj = std::get_if<Object*>(&object)) {
        if ((*obj)->type == ObjectType::ARRAY) {
            auto array = static_cast<NotaArray*>(*obj);
            if (std::holds_alternative<int>(index_val)) {
                int index = std::get<int>(index_val);
                if (array->capacity != -1 && index >= array->capacity) {
                    throw RuntimeError(expr->bracket, "Array index out of bounds for static array.");
                }
                if (index >= 0 && index < array->elements.size()) {
                    lastValue_ = array->elements[index];
                    stack_.push_back(lastValue_);
                    return;
                }
            }
            throw RuntimeError(expr->bracket, "Array index must be a non-negative integer.");
        } else if ((*obj)->type == ObjectType::STRING) {
            auto str = static_cast<NotaString*>(*obj);
            if (std::holds_alternative<int>(index_val)) {
                int index = std::get<int>(index_val);
                if (index >= 0 && index < str->value.length()) {
                    lastValue_ = vm.newObject<NotaString>(std::string(1, str->value[index]));
                    stack_.push_back(lastValue_);
                    return;
                }
            }
            throw RuntimeError(expr->bracket, "String index out of bounds.");
        }
    }

    throw RuntimeError(expr->bracket, "Can only subscript arrays and strings.");
}

void Interpreter::visit(const std::shared_ptr<LogicalExpr>& expr) {
    Value left = evaluate(expr->left);
    stack_.pop_back();

    if (expr->op.type == TokenType::OR) {
        if (isTruthy(left)) {
            lastValue_ = left;
            stack_.push_back(lastValue_);
            return;
        }
    } else { // AND
        if (!isTruthy(left)) {
            lastValue_ = left;
            stack_.push_back(lastValue_);
            return;
        }
    }

    lastValue_ = evaluate(expr->right);
    stack_.pop_back();
    stack_.push_back(lastValue_);
}

void Interpreter::visit(const std::shared_ptr<TypeExpr>& expr) {
    // This should not be called directly during normal execution
    throw RuntimeError(expr->name, "Cannot evaluate a type expression.");
}

void Interpreter::visit(const std::shared_ptr<CastExpr>& expr) {
    Value value = evaluate(expr->expression);
    stack_.pop_back();
    std::string type_name = expr->type->name.lexeme;

    if (expr->type->is_array) {
        // For now, only allow casting to a generic array type without checking element types
        if (auto* obj = std::get_if<Object*>(&value)) {
            if ((*obj)->type == ObjectType::ARRAY) {
                lastValue_ = value;
                stack_.push_back(lastValue_);
                return;
            }
        }
        throw RuntimeError(expr->type->name, "Can only cast arrays to array types.");
    }

    if (type_name == "int") {
        if (std::holds_alternative<double>(value)) {
            lastValue_ = static_cast<int>(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            lastValue_ = static_cast<int>(std::get<bool>(value));
        } else if (auto* obj = std::get_if<Object*>(&value)) {
            if ((*obj)->type == ObjectType::STRING) {
                try {
                    lastValue_ = std::stoi(static_cast<NotaString*>(*obj)->value);
                } catch (const std::invalid_argument&) {
                    throw RuntimeError(expr->type->name, "Invalid string to int cast.");
                }
            } else {
                throw RuntimeError(expr->type->name, "Invalid cast to int.");
            }
        } else {
            lastValue_ = value; // No conversion needed
        }
    } else if (type_name == "float") {
        if (std::holds_alternative<int>(value)) {
            lastValue_ = static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<bool>(value)) {
            lastValue_ = static_cast<double>(std::get<bool>(value));
        } else if (auto* obj = std::get_if<Object*>(&value)) {
            if ((*obj)->type == ObjectType::STRING) {
                 try {
                    lastValue_ = std::stod(static_cast<NotaString*>(*obj)->value);
                } catch (const std::invalid_argument&) {
                    throw RuntimeError(expr->type->name, "Invalid string to float cast.");
                }
            } else {
                throw RuntimeError(expr->type->name, "Invalid cast to float.");
            }
        } else {
            lastValue_ = value; // No conversion needed
        }
    } else if (type_name == "string") {
        if (std::holds_alternative<int>(value)) {
            lastValue_ = vm.newObject<NotaString>(std::to_string(std::get<int>(value)));
        } else if (std::holds_alternative<double>(value)) {
            lastValue_ = vm.newObject<NotaString>(std::to_string(std::get<double>(value)));
        } else if (std::holds_alternative<bool>(value)) {
            lastValue_ = vm.newObject<NotaString>(std::get<bool>(value) ? "true" : "false");
        } else if (auto* obj = std::get_if<Object*>(&value)) {
            if ((*obj)->type == ObjectType::STRING) {
                lastValue_ = value; // No conversion needed
            } else if (auto* instance = dynamic_cast<NotaInstance*>(*obj)) {
                if (auto* method = instance->getClass()->findMethod("to_string")) {
                    lastValue_ = method->bind(*this, instance)->call(*this, {});
                } else {
                    lastValue_ = vm.newObject<NotaString>(instance->toString());
                }
            } else {
                throw RuntimeError(expr->type->name, "Invalid cast to string.");
            }
        } else {
             throw RuntimeError(expr->type->name, "Invalid cast to string.");
        }
    } else if (type_name == "bool") {
        lastValue_ = isTruthy(value);
    } else {
        throw RuntimeError(expr->type->name, "Invalid cast type.");
    }

    stack_.push_back(lastValue_);
}

void Interpreter::visit(const std::shared_ptr<ForEachStmt>& stmt) {
    Value collection = evaluate(stmt->collection);
    stack_.pop_back();

    if (auto obj = std::get_if<Object*>(&collection)) {
        if ((*obj)->type == ObjectType::ARRAY) {
            auto array = static_cast<NotaArray*>(*obj);
            for (const auto& element : array->elements) {
                Environment* loopEnvironment = vm.newObject<Environment>(environment_);
                loopEnvironment->define(stmt->variable.lexeme, element, false, nullptr);
                executeBlock({stmt->body}, loopEnvironment);
            }
            return;
        }
    }

    throw RuntimeError(stmt->variable, "Can only iterate over arrays.");
}

void Interpreter::visit(const std::shared_ptr<MatchStmt>& stmt) {
    Value value = evaluate(stmt->expression);
    stack_.pop_back();

    bool matched = false;
    for (const auto& case_branch : stmt->cases) {
        if (case_branch.is_default) {
            execute(case_branch.body);
            matched = true;
            break;
        }

        for (const auto& case_value_expr : case_branch.values) {
            Value case_value = evaluate(case_value_expr);
            stack_.pop_back();
            if (value == case_value) {
                execute(case_branch.body);
                matched = true;
                break;
            }
        }
        if (matched) {
            break;
        }
    }
}

void Interpreter::checkType(Value& value, const std::shared_ptr<TypeExpr>& type, bool allow_implicit) {
    std::string type_name = type->name.lexeme;

    if (allow_implicit) {
        if (type_name == "float" && std::holds_alternative<int>(value)) {
            value = static_cast<double>(std::get<int>(value));
            return;
        }
        if (type_name == "int" && std::holds_alternative<double>(value)) {
            value = static_cast<int>(std::get<double>(value));
            return;
        }
        if (type_name == "bool") {
            value = isTruthy(value);
            return;
        }
        if (type_name == "int" && std::holds_alternative<bool>(value)) {
            value = static_cast<int>(std::get<bool>(value));
            return;
        }
        if (type_name == "float" && std::holds_alternative<bool>(value)) {
            value = static_cast<double>(std::get<bool>(value));
            return;
        }
    }

    bool type_matches = false;
    if (type->is_array) {
        if (auto* obj = std::get_if<Object*>(&value)) {
            if ((*obj)->type == ObjectType::ARRAY) {
                auto* array = static_cast<NotaArray*>(*obj);
                auto element_type = std::make_shared<TypeExpr>(type->name);
                for (auto& element : array->elements) {
                    checkType(element, element_type, allow_implicit);
                }
                type_matches = true;
            }
        }
    } else {
        if (type_name == "int" && std::holds_alternative<int>(value)) type_matches = true;
        else if (type_name == "float" && std::holds_alternative<double>(value)) type_matches = true;
        else if (type_name == "bool" && std::holds_alternative<bool>(value)) type_matches = true;
        else if (type_name == "string" && std::holds_alternative<Object*>(value) && std::get<Object*>(value)->type == ObjectType::STRING) type_matches = true;
        else if (std::holds_alternative<std::monostate>(value)) type_matches = true; // allow none
    }

    if (!type_matches) {
        throw RuntimeError(type->name, "Type mismatch.");
    }
}

} // namespace nota
