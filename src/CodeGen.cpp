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
        size_t idxL = 0, idxR = 0;
        double l = 0, r = 0;

        bool leftIsNum = false, rightIsNum = false;

        try { l = std::stod(left, &idxL); leftIsNum = true; } catch(...) {}
        try { r = std::stod(right, &idxR); rightIsNum = true; } catch(...) {}

        // If one is boolean string "true"/"false", convert to 1/0 for logic?
        // Or if logic operators (&& ||) handle them.
        // Current parser supports binary ops. Lexer has && ||.
        // Parser creates BinaryExpr. Evaluator visits BinaryExpr.
        // We need to handle &&, || here or inside try/catch?
        // Let's handle logic operators specifically.

        if (expr.op.type == TokenType::AmpAmp) {
             bool lTrue = (left == "true" || (left != "false" && left != "" && left != "0"));
             bool rTrue = (right == "true" || (right != "false" && right != "" && right != "0"));
             return (lTrue && rTrue) ? "true" : "false";
        }
        if (expr.op.type == TokenType::PipePipe) {
             bool lTrue = (left == "true" || (left != "false" && left != "" && left != "0"));
             bool rTrue = (right == "true" || (right != "false" && right != "" && right != "0"));
             return (lTrue || rTrue) ? "true" : "false";
        }

        if (!leftIsNum || !rightIsNum) throw std::runtime_error("Not numbers");

        std::string unitL = left.substr(idxL);
        std::string unitR = right.substr(idxR);

        if (expr.op.type == TokenType::Star) {
            if (!unitL.empty() && !unitR.empty()) throw std::runtime_error("Complex unit math");
            double res = l * r;
            return std::to_string(res) + (unitL.empty() ? unitR : unitL);
        }
        if (expr.op.type == TokenType::Slash) {
            if (!unitR.empty()) throw std::runtime_error("Div by unit");
            double res = (r != 0) ? l / r : 0;
            return std::to_string(res) + unitL;
        }

        if (unitL == unitR || (unitL.empty() && unitR.empty())) {
             double res = 0;
             if (expr.op.type == TokenType::Plus) res = l + r;
             else if (expr.op.type == TokenType::Minus) res = l - r;

             std::string resStr = std::to_string(res);
             resStr.erase(resStr.find_last_not_of('0') + 1, std::string::npos);
             if (resStr.back() == '.') resStr.pop_back();

             return resStr + unitL;
        }

        throw std::runtime_error("Mixed units");

    } catch (...) {
        if (expr.op.type == TokenType::Plus || expr.op.type == TokenType::Minus ||
            expr.op.type == TokenType::Star || expr.op.type == TokenType::Slash) {

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
    auto it = context.find(expr.name.value);
    if (it != context.end()) {
        return it->second;
    }
    return expr.name.value; // Fallback to name (for CSS keywords like 'red')
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

static PropOutput processProperties(const ComponentNode& node, const Context& ctx) {
    PropOutput out;
    std::stringstream cssSS;
    std::stringstream attrSS;
    Evaluator evaluator(ctx);

    // Check for explicit position property first
    bool hasExplicitPosition = false;
    for (const auto& prop : node.properties) {
        if (prop.name == "position") {
            hasExplicitPosition = true;
            break;
        }
    }

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
            attrSS << " onclick=\"" << value << "\"";
            continue;
        }

        if (name == "onHover") {
             attrSS << " onmouseenter=\"" << value << "\"";
             continue;
        }

        // CSS Properties
        if (name == "x") {
            name = "left";
            needsAbsPos = true;
        } else if (name == "y") {
            name = "top";
            needsAbsPos = true;
        } else if (name == "position") {
             if (value == "center") {
                cssSS << "left: 50%; top: 50%; transform: translate(-50%, -50%); position: absolute; ";
                continue;
            } else if (value == "left top" || value == "top left") {
                cssSS << "left: 0; top: 0; position: absolute; ";
                continue;
            }
            if (value == "absolute") needsAbsPos = false;
        }

        if (name == "color") name = "background-color";
        if (name == "radius") name = "border-radius";
        if (name == "spacing") name = "gap";
        if (name == "index") name = "z-index";

        bool isNumber = !value.empty() && std::all_of(value.begin(), value.end(), [](char c){
            return isdigit(c) || c == '.';
        });

        if (isNumber) {
             value += "px";
        }

        cssSS << name << ": " << value << "; ";
    }

    if (needsAbsPos && !hasExplicitPosition) {
        if (cssSS.str().find("position:") == std::string::npos) {
             cssSS << "position: absolute; ";
        }
    }

    out.css = cssSS.str();
    out.attr = attrSS.str();
    return out;
}

// Build context for a component (properties including overrides and definitions)
// Note: In a real system, scope might be inherited or specific.
// For now, we mix defined properties and assigned properties.
static Context buildContext(const ComponentNode& node, const Context& parentCtx) {
    Context ctx = parentCtx; // Inherit parent scope? Usually variables are local or explicitly passed.
    // Nota.md: "property bool isFlag: false". used in "if (isFlag)".
    // So it's local scope.
    // But what about "width: parent.width"? That's different (parent access).
    // Let's start with local scope + simple inheritance for now (or just local).

    // First, add property definitions (defaults)
    Evaluator eval(parentCtx); // Defaults evaluated in parent context? Or empty?
    for (const auto& def : node.propertyDefs) {
        if (def.defaultValue) {
             ctx[def.name] = eval.evaluate(*def.defaultValue);
        }
    }

    // Then add assigned properties (overrides)
    // We need to evaluate them.
    // NOTE: Self-reference? "width: 100; height: width".
    // This requires multipass or lazy eval.
    // For MVP, simplistic order-dependent evaluation or just basic values.
    for (const auto& prop : node.properties) {
        // Evaluate value in parent context? Or current?
        // Usually properties are expressions evaluated in context where they are defined?
        // But "width: 100" -> 100.
        // If "width: isFlag ? 100 : 200", isFlag is local.
        // So we need local context.
        // But we are building it.
        // Let's assume for now properties are evaluated with currently known context.
        ctx[prop.name] = eval.evaluate(*prop.value);
    }
    return ctx;
}

std::string CodeGen::generateHTML(const Node& root, const Context& parentContext) {
    std::stringstream ss;

    if (auto* comp = dynamic_cast<const ComponentNode*>(&root)) {
        std::string tag = "div";
        std::string cssClass = toCssClass(comp->type);

        if (comp->type == "App") {
            tag = "body";
        } else if (comp->type == "Text") {
            tag = "span";
        }

        Context ctx = buildContext(*comp, parentContext);
        auto props = processProperties(*comp, ctx);

        ss << "<" << tag << " class=\"" << cssClass << "\"" << props.attr;

        if (!props.css.empty()) {
            ss << " style=\"" << props.css << "\"";
        }

        ss << ">";

        if (comp->type == "Text") {
            ss << props.content;
        }

        for (const auto& child : comp->children) {
            ss << generateHTML(*child, ctx); // Pass context to children
        }

        ss << "</" << tag << ">";
    } else if (auto* ifNode = dynamic_cast<const IfNode*>(&root)) {
        Evaluator eval(parentContext); // Evaluate condition in current scope
        std::string cond = eval.evaluate(*ifNode->condition);

        bool isTrue = (cond == "true" || (cond != "false" && cond != "" && cond != "0"));

        if (isTrue) {
            for (const auto& child : ifNode->thenBranch) {
                ss << generateHTML(*child, parentContext);
            }
        } else {
            for (const auto& child : ifNode->elseBranch) {
                ss << generateHTML(*child, parentContext);
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
