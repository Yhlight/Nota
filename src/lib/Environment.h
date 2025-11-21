#pragma once

#include "Token.h"
#include <map>
#include <string>
#include <variant>

namespace nota {

class Environment {
public:
    using Value = std::variant<std::monostate, int, double, std::string, bool>;

    void define(const std::string& name, const Value& value);
    Value get(const Token& name);
    void assign(const Token& name, const Value& value);

private:
    std::map<std::string, Value> values_;
};

} // namespace nota
