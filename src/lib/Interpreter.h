/**
 * @file Interpreter.h
 * @brief The AST-walking interpreter for the Nota language.
 */

#pragma once

#include "AST.h"
#include "Environment.h"
#include "VM.h"
#include "FFI.h"
#include <memory>
#include <stdexcept>
#include <vector>
#include <map>
#include <functional>

namespace nota {

class ModuleLoader; // Forward declaration

/**
 * @class Interpreter
 * @brief The AST-walking interpreter for the Nota language.
 */
class Interpreter : public StmtVisitor, public ExprVisitor {
public:
    /**
     * @class RuntimeError
     * @brief An exception thrown at runtime.
     */
    class RuntimeError : public std::runtime_error {
    public:
        RuntimeError(const Token& token, const std::string& message)
            : std::runtime_error(message), token(token) {}
        const Token token;
    };

    /**
     * @class ReturnControl
     * @brief An exception used to unwind the stack for a return statement.
     */
    class ReturnControl : public std::exception {
    public:
        ReturnControl(Value value) : value(value) {}
        const Value value;
    };

    Interpreter(VM& vm);
    ~Interpreter(); // Add destructor declaration

    /**
     * @brief Interpret a vector of statements.
     * @param statements The statements to interpret.
     */
    void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);

    /**
     * @brief Execute a block of statements in a new environment.
     * @param statements The statements to execute.
     * @param environment The environment to execute in.
     */
    void executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, Environment* environment);

    /**
     * @brief Get the current environment.
     * @return The current environment.
     */
    Environment* getEnvironment() { return environment_; }

    /**
     * @brief Get the current package name.
     * @return The current package name.
     */
    std::string getPackageName() const { return packageName_; }

    /**
     * @brief Mark all roots for the garbage collector.
     */
    void markRoots();

    /**
     * @brief Register a native C++ function with the interpreter.
     * @tparam R The return type of the function.
     * @tparam Args The argument types of the function.
     * @param name The name of the function.
     * @param func The function to register.
     */
    template<typename R, typename... Args>
    void registerNative(const std::string& name, R (*func)(Args...));

    /**
     * @brief Register a native C++ function with the interpreter.
     * @param name The name of the function.
     * @param arity The arity of the function.
     * @param fn The function to register.
     */
    void registerNative(const std::string& name, int arity, NotaNativeFunction::NativeFn fn);

    void visit(const std::shared_ptr<Block>& stmt) override;
    void visit(const std::shared_ptr<ExpressionStmt>& stmt) override;
    void visit(const std::shared_ptr<VarStmt>& stmt) override;
    void visit(const std::shared_ptr<IfStmt>& stmt) override;
    void visit(const std::shared_ptr<WhileStmt>& stmt) override;
    void visit(const std::shared_ptr<DoWhileStmt>& stmt) override;
    void visit(const std::shared_ptr<FunctionStmt>& stmt) override;
    void visit(const std::shared_ptr<ReturnStmt>& stmt) override;
    void visit(const std::shared_ptr<ClassStmt>& stmt) override;
    void visit(const std::shared_ptr<ImportStmt>& stmt) override;
    void visit(const std::shared_ptr<PackageStmt>& stmt) override;
    void visit(const std::shared_ptr<ForEachStmt>& stmt) override;

    void visit(const std::shared_ptr<Binary>& expr) override;
    void visit(const std::shared_ptr<Grouping>& expr) override;
    void visit(const std::shared_ptr<Literal>& expr) override;
    void visit(const std::shared_ptr<Unary>& expr) override;
    void visit(const std::shared_ptr<Variable>& expr) override;
    void visit(const std::shared_ptr<Assign>& expr) override;
    void visit(const std::shared_ptr<Postfix>& expr) override;
    void visit(const std::shared_ptr<CallExpr>& expr) override;
    void visit(const std::shared_ptr<GetExpr>& expr) override;
    void visit(const std::shared_ptr<SetExpr>& expr) override;
    void visit(const std::shared_ptr<ThisExpr>& expr) override;
    void visit(const std::shared_ptr<ScopeAccessExpr>& expr) override;
    void visit(const std::shared_ptr<LambdaExpr>& expr) override;
    void visit(const std::shared_ptr<ArrayExpr>& expr) override;
    void visit(const std::shared_ptr<SubscriptExpr>& expr) override;
    void visit(const std::shared_ptr<LogicalExpr>& expr) override;
    void visit(const std::shared_ptr<TypeExpr>& expr) override;
    void visit(const std::shared_ptr<CastExpr>& expr) override;

    VM& vm;

    void checkType(Value& value, const std::shared_ptr<TypeExpr>& type, bool allow_implicit = false);

private:
    void execute(const std::shared_ptr<Stmt>& stmt);
    Value evaluate(const std::shared_ptr<Expr>& expr);
    bool isTruthy(const Value& value);

    Value lastValue_;
    Environment* environment_;
    std::unique_ptr<ModuleLoader> moduleLoader_;
    std::map<std::string, std::shared_ptr<Interpreter>> modules_;
    std::vector<std::shared_ptr<Stmt>> statements_;
    std::vector<Value> stack_;
    std::string packageName_;
};

template<typename R, typename... Args>
void Interpreter::registerNative(const std::string& name, R (*func)(Args...)) {
    registerNative(name, sizeof...(Args), [this, func](Interpreter&, std::vector<Value> args) {
        try {
            return ffi::NativeCaller<R, Args...>::Call(vm, func, args);
        } catch (const std::exception& e) {
            vm.raiseError(e.what());
            return Value{};
        }
    });
}

} // namespace nota
