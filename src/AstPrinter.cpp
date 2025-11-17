#include "AST/AstPrinter.h"
#include <sstream>

namespace nota {
namespace ast {

    std::string AstPrinter::print(const std::vector<std::unique_ptr<Stmt>>& statements) {
        std::stringstream ss;
        for (const auto& stmt : statements) {
            ss << std::any_cast<std::string>(stmt->accept(*this));
        }
        return ss.str();
    }

    std::any AstPrinter::visit(class VarDeclStmt& stmt) {
        std::stringstream ss;
        ss << "(var-decl " << stmt.name.lexeme;
        if (stmt.type) {
            ss << " " << stmt.type->lexeme;
        }
        if (stmt.initializer) {
            ss << " " << std::any_cast<std::string>(stmt.initializer->accept(*this));
        }
        ss << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class BlockStmt& stmt) {
        std::stringstream ss;
        ss << "(block";
        for (const auto& s : stmt.statements) {
            ss << " " << std::any_cast<std::string>(s->accept(*this));
        }
        ss << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class IfStmt& stmt) {
        std::stringstream ss;
        ss << "(if " << std::any_cast<std::string>(stmt.condition->accept(*this));
        ss << " " << std::any_cast<std::string>(stmt.then_branch->accept(*this));
        if (stmt.else_branch) {
            ss << " " << std::any_cast<std::string>(stmt.else_branch->accept(*this));
        }
        ss << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class WhileStmt& stmt) {
        std::stringstream ss;
        ss << "(while " << std::any_cast<std::string>(stmt.condition->accept(*this));
        ss << " " << std::any_cast<std::string>(stmt.body->accept(*this));
        ss << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class DoWhileStmt& stmt) {
        std::stringstream ss;
        ss << "(do-while " << std::any_cast<std::string>(stmt.body->accept(*this));
        ss << " " << std::any_cast<std::string>(stmt.condition->accept(*this));
        ss << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class ExpressionStmt& stmt) {
        return stmt.expression->accept(*this);
    }

    std::any AstPrinter::visit(class ForEachStmt& stmt) {
        std::stringstream ss;
        ss << "(for-each " << stmt.variable.lexeme;
        ss << " " << std::any_cast<std::string>(stmt.container->accept(*this));
        ss << " " << std::any_cast<std::string>(stmt.body->accept(*this));
        ss << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class ForStmt& stmt) {
        std::stringstream ss;
        ss << "(for";
        if (stmt.initializer) {
            ss << " " << std::any_cast<std::string>(stmt.initializer->accept(*this));
        }
        if (stmt.condition) {
            ss << " " << std::any_cast<std::string>(stmt.condition->accept(*this));
        }
        if (stmt.increment) {
            ss << " " << std::any_cast<std::string>(stmt.increment->accept(*this));
        }
        ss << " " << std::any_cast<std::string>(stmt.body->accept(*this));
        ss << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class MatchStmt& stmt) {
        std::stringstream ss;
        ss << "(match " << std::any_cast<std::string>(stmt.expression->accept(*this));
        for (const auto& c : stmt.cases) {
            ss << " (case";
            for (const auto& v : c.values) {
                ss << " " << std::any_cast<std::string>(v->accept(*this));
            }
            ss << " " << std::any_cast<std::string>(c.body->accept(*this));
            ss << ")";
        }
        if (stmt.else_branch) {
            ss << " " << std::any_cast<std::string>(stmt.else_branch->accept(*this));
        }
        ss << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class FuncDeclStmt& stmt) {
        std::stringstream ss;
        ss << "(func-decl " << stmt.name.lexeme << " (";
        for (size_t i = 0; i < stmt.params.size(); ++i) {
            ss << stmt.params[i].name.lexeme;
            if (stmt.params[i].type) {
                ss << ": " << stmt.params[i].type->lexeme;
            }
            if (i < stmt.params.size() - 1) {
                ss << " ";
            }
        }
        ss << ") ";
        if (stmt.return_type) {
            ss << stmt.return_type->lexeme << " ";
        }
        ss << std::any_cast<std::string>(stmt.body->accept(*this)) << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class ReturnStmt& stmt) {
        std::stringstream ss;
        ss << "(return " << std::any_cast<std::string>(stmt.value->accept(*this)) << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class LiteralExpr& expr) {
        return expr.value.lexeme;
    }

    std::any AstPrinter::visit(class UnaryExpr& expr) {
        std::stringstream ss;
        ss << "(" << expr.op.lexeme << " " << std::any_cast<std::string>(expr.right->accept(*this)) << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class BinaryExpr& expr) {
        std::stringstream ss;
        ss << "(" << expr.op.lexeme << " " << std::any_cast<std::string>(expr.left->accept(*this)) << " " << std::any_cast<std::string>(expr.right->accept(*this)) << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class VariableExpr& expr) {
        return expr.name.lexeme;
    }

    std::any AstPrinter::visit(class AssignExpr& expr) {
        std::stringstream ss;
        ss << "(assign " << expr.name.lexeme << " " << std::any_cast<std::string>(expr.value->accept(*this)) << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class PostfixExpr& expr) {
        std::stringstream ss;
        ss << "(" << std::any_cast<std::string>(expr.left->accept(*this)) << " " << expr.op.lexeme << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class CallExpr& expr) {
        std::stringstream ss;
        ss << "(call " << std::any_cast<std::string>(expr.callee->accept(*this));
        for (const auto& arg : expr.arguments) {
            ss << " " << std::any_cast<std::string>(arg->accept(*this));
        }
        ss << ")";
        return ss.str();
    }

    std::any AstPrinter::visit(class LambdaExpr& expr) {
        std::stringstream ss;
        ss << "(lambda (";
        for (size_t i = 0; i < expr.params.size(); ++i) {
            ss << expr.params[i].name.lexeme;
            if (expr.params[i].type) {
                ss << ": " << expr.params[i].type->lexeme;
            }
            if (i < expr.params.size() - 1) {
                ss << " ";
            }
        }
        ss << ") ";
        if (std::holds_alternative<std::unique_ptr<Expr>>(expr.body)) {
            ss << std::any_cast<std::string>(std::get<std::unique_ptr<Expr>>(expr.body)->accept(*this));
        } else {
            ss << std::any_cast<std::string>(std::get<std::unique_ptr<Stmt>>(expr.body)->accept(*this));
        }
        ss << ")";
        return ss.str();
    }

} // namespace ast
} // namespace nota
