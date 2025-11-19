#pragma once

#include "Token.h"
#include <map>
#include <string>
#include <variant>
#include <memory>

class Environment {
public:
    Environment();
    explicit Environment(std::shared_ptr<Environment> enclosing);

    void define(const std::string& name, const std::variant<std::monostate, int, double, std::string, bool>& value, bool isMutable);
    void assign(const Token& name, const std::variant<std::monostate, int, double, std::string, bool>& value);
    const std::variant<std::monostate, int, double, std::string, bool>& get(const Token& name);

private:
    struct Variable {
        std::variant<std::monostate, int, double, std::string, bool> value;
        bool isMutable;
    };

    std::map<std::string, Variable> values;
    std::shared_ptr<Environment> enclosing;
};
