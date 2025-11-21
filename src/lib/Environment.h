#pragma once

#include "Token.h"
#include <map>
#include <string>
#include "AST.h"
#include <memory>

namespace nota {

class Environment {
public:
    Environment(std::shared_ptr<Environment> enclosing = nullptr) : enclosing_(enclosing) {}

    void define(const std::string& name, const Value& value);
    Value get(const Token& name);
    void assign(const Token& name, const Value& value);

private:
    std::shared_ptr<Environment> enclosing_;
    std::map<std::string, Value> values_;
};

} // namespace nota
