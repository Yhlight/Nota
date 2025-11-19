#pragma once

#include "NotaCallable.h"
#include "Stmt.h"
#include "Environment.h"
#include <vector>
#include <any>
#include <memory>

class NotaFunction : public NotaCallable {
public:
    NotaFunction(std::shared_ptr<const FunctionStmt> declaration, std::shared_ptr<Environment> closure);
    ~NotaFunction();
    int arity() override;
    std::any call(Interpreter& interpreter, std::vector<std::any> arguments) override;

private:
    std::shared_ptr<const FunctionStmt> declaration;
    std::shared_ptr<Environment> closure;
};
