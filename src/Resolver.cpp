#include "Resolver.h"
#include <iostream>

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        resolve(*statement);
    }
}

const std::map<std::string, const ItemStmt*>& Resolver::get_custom_types() const {
    return custom_types;
}

void Resolver::resolve(const Stmt& stmt) {
    stmt.accept(*this);
}

void Resolver::resolve(const Expr& expr) {
    expr.accept(*this);
}

std::any Resolver::visit(const ComponentStmt& stmt) {
    if (custom_types.count(stmt.name.lexeme)) {
        // This is an instantiation of a custom component.
        // We will handle this in the code generator.
    }

    begin_scope();
    for (const auto& statement : stmt.body) {
        resolve(*statement);
    }
    end_scope();
    return {};
}

std::any Resolver::visit(const ItemStmt& stmt) {
    if (custom_types.count(stmt.name.lexeme)) {
        // In a more robust compiler, we would report an error here.
    }
    custom_types[stmt.name.lexeme] = &stmt;
    begin_scope();
    for (const auto& statement : stmt.body) {
        resolve(*statement);
    }
    end_scope();
    return {};
}

std::any Resolver::visit(const PropertyStmt& stmt) {
    if (stmt.name.lexeme == "id") {
        if (auto* id_expr = dynamic_cast<IdentifierExpr*>(stmt.value.get())) {
            declare(id_expr->name);
            define(id_expr->name);
        }
    } else {
        resolve(*stmt.value);
    }
    return {};
}

std::any Resolver::visit(const LiteralExpr& expr) {
    return {};
}

std::any Resolver::visit(const IdentifierExpr& expr) {
    if (!scopes.empty()) {
        auto& scope = scopes.top();
        if (scope.count(expr.name.lexeme) && !scope.at(expr.name.lexeme)) {
             std::cerr << "Resolver Error: Cannot read local variable in its own initializer." << std::endl;
        }
    }
    return {};
}

std::any Resolver::visit(const BinaryExpr& expr) {
    resolve(*expr.left);
    resolve(*expr.right);
    return {};
}

std::any Resolver::visit(const GetExpr& expr) {
    resolve(*expr.object);
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

void Resolver::define(const Token& name) {
    if (scopes.empty()) {
        return;
    }
    scopes.top()[name.lexeme] = true;
}
