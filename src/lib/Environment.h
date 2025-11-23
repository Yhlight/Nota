#pragma once

#include "Token.h"
#include <map>
#include <string>
#include "NotaObjects.h"
#include <memory>
#include "AST.h"

namespace nota {

struct VariableInfo {
    Value value;
    bool is_mutable;
};

class Environment : public Object {
public:
    Environment(Environment* enclosing = nullptr)
        : Object(ObjectType::ENVIRONMENT), enclosing_(enclosing) {}

    void define(const std::string& name, const Value& value, bool is_mutable, std::shared_ptr<VarStmt> stmt = nullptr);
    Value get(const Token& name);
    void assign(const Token& name, const Value& value);
    bool isDefined(const std::string& name);
    std::shared_ptr<VarStmt> findVariable(const Token& name);

    void traceReferences(VM& vm) override;
    size_t size() const override;

private:
    Environment* enclosing_;
    std::map<std::string, VariableInfo> values_;
    std::map<std::string, std::shared_ptr<VarStmt>> statements_;
};

} // namespace nota
