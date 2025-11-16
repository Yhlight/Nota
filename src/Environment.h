#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "Token.h"
#include <any>
#include <map>
#include <string>

class Environment {
public:
    Environment();

    void define(const std::string& name, const std::any& value);
    std::any get(const Token& name);

private:
    std::map<std::string, std::any> values;
};

#endif // ENVIRONMENT_H
