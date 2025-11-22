#pragma once

#include "Token.h"
#include <map>
#include <string>
#include "NotaObjects.h"
#include <memory>

namespace nota {

class Environment : public Object {
public:
    Environment(Environment* enclosing = nullptr) : enclosing_(enclosing) {}

    void define(const std::string& name, const Value& value);
    Value get(const Token& name);
    void assign(const Token& name, const Value& value);

    void traceReferences(VM& vm) override;

private:
    Environment* enclosing_;
    std::map<std::string, Value> values_;
};

} // namespace nota
