#pragma once

#include "Token.h"
#include <map>
#include <string>
#include <variant>
#include <memory>

namespace nota {

class Callable; // Forward declaration

class Environment {
public:
    using Value = std::variant<std::monostate, int, double, std::string, bool, std::shared_ptr<Callable>>;

    Environment(Environment* enclosing = nullptr) : enclosing_(enclosing) {}

    void define(const std::string& name, const Value& value);
    Value get(const Token& name);
    void assign(const Token& name, const Value& value);

private:
    Environment* enclosing_;
    std::map<std::string, Value> values_;
};

} // namespace nota
