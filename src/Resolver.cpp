#include "Resolver.h"

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        statement->accept(*this);
    }
}

std::any Resolver::visit(const ComponentStmt& stmt) {
    begin_scope();
    for (const auto& statement : stmt.body) {
        statement->accept(*this);
    }
    end_scope();
    return {};
}

std::any Resolver::visit(const PropertyStmt& stmt) {
    stmt.value->accept(*this);
    if (stmt.name.lexeme == "id") {
        if (auto* id_expr = dynamic_cast<IdentifierExpr*>(stmt.value.get())) {
            declare(id_expr->name);
        }
    }
    return {};
}

std::any Resolver::visit(const LiteralExpr& expr) {
    return {};
}

std::any Resolver::visit(const IdentifierExpr& expr) {
    return {};
}

void Resolver::begin_scope() {
    scopes.emplace();
}

void Resolver::end_scope() {
    scopes.pop();
}

void Resolver::declare(const Token& name) {
    if (scopes.empty()) {
        return;
    }

    std::map<std::string, bool>& scope = scopes.top();
    if (scope.count(name.lexeme)) {
        // In a more robust compiler, we would report an error here.
        // For now, we'll just ignore re-declarations.
    }
    scope[name.lexeme] = false;
}
