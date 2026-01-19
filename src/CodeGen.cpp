#include "CodeGen.h"
#include <sstream>
#include <algorithm>
#include <vector>
#include <iostream>

// Evaluation logic implementation
std::string Evaluator::evaluate(Expr& expr) {
    return expr.accept(*this);
}

std::string Evaluator::visitBinaryExpr(BinaryExpr& expr) {
    std::string left = evaluate(*expr.left);
    std::string right = evaluate(*expr.right);

    try {
        size_t idxL, idxR;
        double l = std::stod(left, &idxL);
        double r = std::stod(right, &idxR);

        std::string unitL = left.substr(idxL);
        std::string unitR = right.substr(idxR);

        double res = 0;
        if (expr.op.type == TokenType::Plus) res = l + r;
        else if (expr.op.type == TokenType::Minus) res = l - r;
        else if (expr.op.type == TokenType::Star) res = l * r;
        else if (expr.op.type == TokenType::Slash) res = (r != 0) ? l / r : 0;

        std::string unit = unitL.empty() ? unitR : unitL;

        std::stringstream ss;
        ss << res << unit;
        return ss.str();

    } catch (...) {
        if (expr.op.type == TokenType::Plus) {
            return left + right;
        } else if (expr.op.type == TokenType::EqualEqual) {
             return (left == right) ? "true" : "false";
        } else if (expr.op.type == TokenType::BangEqual) {
             return (left != right) ? "true" : "false";
        }
        return "";
    }
}

std::string Evaluator::visitLiteralExpr(LiteralExpr& expr) {
    return expr.value;
}

std::string Evaluator::visitVariableExpr(VariableExpr& expr) {
    return expr.name.value;
}

// Helper to convert Nota type to CSS class
static std::string toCssClass(const std::string& type) {
    std::string lowerType = type;
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    return "nota-" + lowerType;
}

static std::string generateInlineStyle(const ComponentNode& node) {
    std::stringstream ss;
    bool hasStyle = false;
    Evaluator evaluator;

    for (const auto& prop : node.properties) {
        if (prop.name == "text") continue; // handled in content

        // Skip id for style
        if (prop.name == "id") continue;

        if (!hasStyle) {
            ss << "style=\"";
            hasStyle = true;
        }

        std::string cssName = prop.name;
        // Evaluate the expression
        std::string value = evaluator.evaluate(*prop.value);

        // Positioning logic
        if (cssName == "x") {
            cssName = "left";
            if (!hasStyle || ss.str().find("position: absolute") == std::string::npos) {
                ss << "position: absolute; ";
            }
        } else if (cssName == "y") {
            cssName = "top";
            if (!hasStyle || ss.str().find("position: absolute") == std::string::npos) {
                ss << "position: absolute; ";
            }
        } else if (cssName == "position") {
            if (value == "center") {
                ss << "left: 50%; top: 50%; transform: translate(-50%, -50%); position: absolute; ";
                continue;
            } else if (value == "left top" || value == "top left") {
                ss << "left: 0; top: 0; position: absolute; ";
                continue;
            }
            continue;
        }

        // Simple mapping
        if (cssName == "color") cssName = "background-color";
        if (cssName == "radius") cssName = "border-radius";
        if (cssName == "spacing") cssName = "gap";

        // Handle units
        bool isNumber = !value.empty() && std::all_of(value.begin(), value.end(), [](char c){
            return isdigit(c) || c == '.';
        });

        if (isNumber && value.find('.') != std::string::npos) {
             value += "px";
        } else if (isNumber) {
             value += "px";
        }

        ss << cssName << ": " << value << "; ";
    }

    if (hasStyle) {
        ss << "\"";
    }
    return ss.str();
}

std::string CodeGen::generateHTML(const Node& root) {
    std::stringstream ss;

    if (auto* comp = dynamic_cast<const ComponentNode*>(&root)) {
        std::string tag = "div";
        std::string cssClass = toCssClass(comp->type);

        if (comp->type == "App") {
            tag = "body";
        } else if (comp->type == "Text") {
            tag = "span";
        }

        ss << "<" << tag << " class=\"" << cssClass << "\"";

        std::string style = generateInlineStyle(*comp);
        if (!style.empty()) {
            ss << " " << style;
        }

        ss << ">";

        // For Text component
        if (comp->type == "Text") {
            for (const auto& prop : comp->properties) {
                if (prop.name == "text") {
                    // Evaluate and output
                    Evaluator eval;
                    ss << eval.evaluate(*prop.value);
                }
            }
        }

        for (const auto& child : comp->children) {
            ss << generateHTML(*child);
        }

        ss << "</" << tag << ">";
    } else if (auto* ifNode = dynamic_cast<const IfNode*>(&root)) {
        Evaluator eval;
        std::string cond = eval.evaluate(*ifNode->condition);

        bool isTrue = (cond == "true" || (cond != "false" && cond != "" && cond != "0"));

        if (isTrue) {
            for (const auto& child : ifNode->thenBranch) {
                ss << generateHTML(*child);
            }
        } else {
            for (const auto& child : ifNode->elseBranch) {
                ss << generateHTML(*child);
            }
        }
    }

    return ss.str();
}

std::string CodeGen::generateCSS(const Node& root) {
    return R"(
.nota-app { margin: 0; padding: 0; width: 100%; height: 100%; }
.nota-rect { display: block; position: relative; }
.nota-text { display: block; position: relative; }
.nota-row { display: flex; flex-direction: row; position: relative; }
.nota-col { display: flex; flex-direction: column; position: relative; }
)";
}
