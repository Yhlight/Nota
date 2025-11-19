#pragma once

#include "Token.h"
#include <map>
#include <string>
#include <any>
#include <memory>

class Environment {
public:
    Environment();
    explicit Environment(std::shared_ptr<Environment> enclosing);

    void define(const std::string& name, const std::any& value = {}, bool isMutable = false);
    void assign(const Token& name, const std::any& value);
    const std::any& get(const Token& name);

private:
    struct Variable {
        std::any value;
        bool isMutable;
    };

    std::map<std::string, Variable> values;
    std::shared_ptr<Environment> enclosing;
};
