#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "Token.h"
#include <any>
#include <map>
#include <string>
#include <memory>

class Environment {
public:
    Environment();
    Environment(std::shared_ptr<Environment> enclosing);

    void define(const std::string& name, const std::any& value);
    std::any get(const Token& name);

private:
    std::shared_ptr<Environment> enclosing;
    std::map<std::string, std::any> values;
};

#endif // ENVIRONMENT_H
