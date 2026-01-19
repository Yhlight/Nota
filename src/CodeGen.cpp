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

    // Check if either side is already a calc() or complex string?
    // For now, let's try to parse as number+unit.

    try {
        size_t idxL = 0, idxR = 0;
        double l = 0, r = 0;

        bool leftIsNum = false, rightIsNum = false;

        try { l = std::stod(left, &idxL); leftIsNum = true; } catch(...) {}
        try { r = std::stod(right, &idxR); rightIsNum = true; } catch(...) {}

        if (!leftIsNum || !rightIsNum) throw std::runtime_error("Not numbers");

        std::string unitL = left.substr(idxL);
        std::string unitR = right.substr(idxR);

        // Handle multiplication/division (usually one scalar)
        if (expr.op.type == TokenType::Star) {
            if (!unitL.empty() && !unitR.empty()) throw std::runtime_error("Complex unit math"); // px * px
            double res = l * r;
            return std::to_string(res) + (unitL.empty() ? unitR : unitL); // Remove trailing zeros? std::to_string keeps them.
        }
        if (expr.op.type == TokenType::Slash) {
            if (!unitR.empty()) throw std::runtime_error("Div by unit"); // px / px is scalar, but px / % is hard.
            double res = (r != 0) ? l / r : 0;
            return std::to_string(res) + unitL;
        }

        // Addition/Subtraction
        if (unitL == unitR || (unitL.empty() && unitR.empty())) {
             double res = 0;
             if (expr.op.type == TokenType::Plus) res = l + r;
             else if (expr.op.type == TokenType::Minus) res = l - r;

             // Remove trailing zeros for cleanliness
             std::string resStr = std::to_string(res);
             resStr.erase(resStr.find_last_not_of('0') + 1, std::string::npos);
             if (resStr.back() == '.') resStr.pop_back();

             return resStr + unitL;
        }

        // Mixed units (e.g. 100% - 20px)
        throw std::runtime_error("Mixed units");

    } catch (...) {
        // Fallback to calc() or string concatenation
        if (expr.op.type == TokenType::Plus || expr.op.type == TokenType::Minus ||
            expr.op.type == TokenType::Star || expr.op.type == TokenType::Slash) {

            // Check if it's string concatenation (strings)
            // If strictly logic, we might want calc.
            // But if one is "solid", it's probably CSS value concat.
            // Heuristic: if contains units (px, %, em, etc) or calc, use calc.
            // If contains alpha chars that are not units, use concat (except calc).

            // Simple heuristic: If operator is + and it looks like string concat?
            // "1px" + "solid" -> "1px solid" (Evaluator currently does this via exception catch block below?)
            // Actually, my previous code did: `if (expr.op.type == TokenType::Plus) return left + right;`
            // That merged "1px" and "solid" into "1pxsolid" (no space).
            // But parser handles space for "1px solid".
            // So if we are here, it's explicit `+` in source. `width: 10 + 20`.
            // If `width: 100% - 20px`, we want `calc(100% - 20px)`.

            // If op is Plus, and it looks like strings (no digits?), concat.
            // But `100%` has digits.

            // Let's assume arithmetic operators +, -, *, / implies CALC if not simple math.
            // UNLESS it is string literal?
            // Parse strings.
            bool isStrL = left.size() >= 2 && left.front() == '"' && left.back() == '"'; // LiteralExpr stores raw value? No, strips quotes usually?
            // Lexer `readString` strips quotes?
            // Lexer: `advance(); // Skip "` ... `value += advance();` ... `advance(); // Skip closing "`
            // So LiteralExpr value does NOT have quotes.

            // If it's not a number, it's a string/identifier.
            // If we have `calc(100% - 20px)`, we return it.

            std::string opStr;
            if (expr.op.type == TokenType::Plus) opStr = "+";
            else if (expr.op.type == TokenType::Minus) opStr = "-";
            else if (expr.op.type == TokenType::Star) opStr = "*";
            else if (expr.op.type == TokenType::Slash) opStr = "/";

            return "calc(" + left + " " + opStr + " " + right + ")";
        }

        if (expr.op.type == TokenType::EqualEqual) {
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

struct PropOutput {
    std::string css;
    std::string attr;
    std::string content;
};

static PropOutput processProperties(const ComponentNode& node) {
    PropOutput out;
    std::stringstream cssSS;
    std::stringstream attrSS;
    Evaluator evaluator;

    // Check for explicit position property first to determine default behavior
    bool hasExplicitPosition = false;
    for (const auto& prop : node.properties) {
        if (prop.name == "position") {
            hasExplicitPosition = true;
            break;
        }
    }

    // Default absolute positioning for x/y/position handling
    bool needsAbsPos = false;

    for (const auto& prop : node.properties) {
        std::string name = prop.name;
        std::string value = evaluator.evaluate(*prop.value);

        if (name == "text") {
            out.content = value;
            continue;
        }

        if (name == "id") {
            attrSS << " id=\"" << value << "\"";
            continue;
        }

        if (name == "onClick") {
            // Simple JS injection. In real world, escape quotes!
            attrSS << " onclick=\"" << value << "\"";
            continue;
        }

        if (name == "onHover") {
             attrSS << " onmouseenter=\"" << value << "\"";
             continue;
        }

        // CSS Properties

        // Positioning sugar
        if (name == "x") {
            name = "left";
            needsAbsPos = true;
        } else if (name == "y") {
            name = "top";
            needsAbsPos = true;
        } else if (name == "position") {
            // Handle "left top", "center"
             if (value == "center") {
                cssSS << "left: 50%; top: 50%; transform: translate(-50%, -50%); position: absolute; ";
                continue;
            } else if (value == "left top" || value == "top left") {
                cssSS << "left: 0; top: 0; position: absolute; ";
                continue;
            }
            // If just "absolute" or "relative", pass through
            if (value == "absolute") needsAbsPos = false; // Already set
        }

        // Mappings
        if (name == "color") name = "background-color";
        if (name == "radius") name = "border-radius";
        if (name == "spacing") name = "gap";

        // Units
        // Check if value is just number
        bool isNumber = !value.empty() && std::all_of(value.begin(), value.end(), [](char c){
            return isdigit(c) || c == '.';
        });

        if (isNumber) {
             value += "px";
        }

        cssSS << name << ": " << value << "; ";
    }

    if (needsAbsPos && !hasExplicitPosition) {
        // Only add if not already present?
        // Simple check
        if (cssSS.str().find("position:") == std::string::npos) {
             cssSS << "position: absolute; ";
        }
    }

    out.css = cssSS.str();
    out.attr = attrSS.str();
    return out;
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

        auto props = processProperties(*comp);

        ss << "<" << tag << " class=\"" << cssClass << "\"" << props.attr;

        if (!props.css.empty()) {
            ss << " style=\"" << props.css << "\"";
        }

        ss << ">";

        if (comp->type == "Text") {
            ss << props.content;
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
