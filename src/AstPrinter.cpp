#include "AST/AstPrinter.h"
#include <sstream>

namespace nota {
namespace ast {

    std::string AstPrinter::print(const std::vector<std::unique_ptr<Stmt>>& statements) {
        std::stringstream ss;
        for (const auto& stmt : statements) {
            ss << stmt->accept(*this);
        }
        return ss.str();
    }

    std::string AstPrinter::visit(class VarDeclStmt& stmt) {
        std::stringstream ss;
        ss << "(var-decl " << stmt.name.lexeme;
        if (stmt.type) {
            ss << " " << stmt.type->lexeme;
        }
        if (stmt.initializer) {
            ss << " " << stmt.initializer->accept(*this);
        }
        ss << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class LambdaExpr& expr) {
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
            ss << std::get<std::unique_ptr<Expr>>(expr.body)->accept(*this);
        } else {
            ss << std::get<std::unique_ptr<Stmt>>(expr.body)->accept(*this);
        }
        ss << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class BlockStmt& stmt) {
        std::stringstream ss;
        ss << "(block";
        for (const auto& s : stmt.statements) {
            ss << " " << s->accept(*this);
        }
        ss << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class IfStmt& stmt) {
        std::stringstream ss;
        ss << "(if " << stmt.condition->accept(*this);
        ss << " " << stmt.then_branch->accept(*this);
        if (stmt.else_branch) {
            ss << " " << stmt.else_branch->accept(*this);
        }
        ss << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class WhileStmt& stmt) {
        std::stringstream ss;
        ss << "(while " << stmt.condition->accept(*this);
        ss << " " << stmt.body->accept(*this);
        ss << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class DoWhileStmt& stmt) {
        std::stringstream ss;
        ss << "(do-while " << stmt.body->accept(*this);
        ss << " " << stmt.condition->accept(*this);
        ss << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class ExpressionStmt& stmt) {
        return stmt.expression->accept(*this);
    }

    std::string AstPrinter::visit(class ForEachStmt& stmt) {
        std::stringstream ss;
        ss << "(for-each " << stmt.variable.lexeme;
        ss << " " << stmt.container->accept(*this);
        ss << " " << stmt.body->accept(*this);
        ss << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class ForStmt& stmt) {
        std::stringstream ss;
        ss << "(for";
        if (stmt.initializer) {
            ss << " " << stmt.initializer->accept(*this);
        }
        if (stmt.condition) {
            ss << " " << stmt.condition->accept(*this);
        }
        if (stmt.increment) {
            ss << " " << stmt.increment->accept(*this);
        }
        ss << " " << stmt.body->accept(*this);
        ss << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class MatchStmt& stmt) {
        std::stringstream ss;
        ss << "(match " << stmt.expression->accept(*this);
        for (const auto& c : stmt.cases) {
            ss << " (case";
            for (const auto& v : c.values) {
                ss << " " << v->accept(*this);
            }
            ss << " " << c.body->accept(*this);
            ss << ")";
        }
        if (stmt.else_branch) {
            ss << " " << stmt.else_branch->accept(*this);
        }
        ss << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class LiteralExpr& expr) {
        return expr.value.lexeme;
    }

    std::string AstPrinter::visit(class UnaryExpr& expr) {
        std::stringstream ss;
        ss << "(" << expr.op.lexeme << " " << expr.right->accept(*this) << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class BinaryExpr& expr) {
        std::stringstream ss;
        ss << "(" << expr.op.lexeme << " " << expr.left->accept(*this) << " " << expr.right->accept(*this) << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class VariableExpr& expr) {
        return expr.name.lexeme;
    }

    std::string AstPrinter::visit(class AssignExpr& expr) {
        std::stringstream ss;
        ss << "(assign " << expr.name.lexeme << " " << expr.value->accept(*this) << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class PostfixExpr& expr) {
        std::stringstream ss;
        ss << "(" << expr.left->accept(*this) << " " << expr.op.lexeme << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class FuncDeclStmt& stmt) {
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
        ss << stmt.body->accept(*this) << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class ReturnStmt& stmt) {
        std::stringstream ss;
        ss << "(return " << stmt.value->accept(*this) << ")";
        return ss.str();
    }

    std::string AstPrinter::visit(class CallExpr& expr) {
        std::stringstream ss;
        ss << "(call " << expr.callee->accept(*this);
        for (const auto& arg : expr.arguments) {
            ss << " " << arg->accept(*this);
        }
        ss << ")";
        return ss.str();
    }

} // namespace ast
} // namespace nota
