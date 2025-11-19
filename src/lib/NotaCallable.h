#pragma once

#include <vector>
#include <any>
#include <memory>

class Interpreter; // Forward declaration

class NotaCallable {
public:
    virtual ~NotaCallable() = default;
    virtual int arity() = 0;
    virtual std::any call(Interpreter& interpreter, std::vector<std::any> arguments) = 0;
};
