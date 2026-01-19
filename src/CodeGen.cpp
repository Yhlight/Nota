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
        // Fallback for non-numeric operations (like CSS value merging)
        // If we have "1px" + "solid", we might want "1px solid".
        // But our Parser merges these into a single LiteralExpr string.
        // So we shouldn't see + here for "1px solid black".

        // Fallback: If we have multiple values joined by space (implicit in expression?),
        // we might not see it here if it's a single LiteralExpr string.
        // If expr is BinaryExpr, we are here.
        if (expr.op.type == TokenType::Plus) {
            return left + right;
        } else {
            // For other operators if not numbers, return empty or what?
            // "1px solid" is a single string literal now.
            // "border: 1px solid" -> Literal("1px solid").
            // CodeGen evaluates Literal("1px solid") -> "1px solid".
            // So `BorderMapping` test failure means `evaluate` failed to return "1px solid black".
            // Why? `html.find` failed.
            // Let's print out what `evaluate` returns in debug or assume it works if not exception.
            // The issue might be `1px solid black` being parsed.
            // The Parser loop consumes Identifier/Number.
            // "1px" (Number/Ident merged), "solid" (Ident), "black" (Ident).
            // It creates one LiteralExpr with "1px solid black".
            // `visitLiteralExpr` returns `expr.value`.
            // So it should work.
            // UNLESS `1px` is handled weirdly by Lexer?
            // Lexer: `readNumber` consumes `.` and digits. `1` is Number.
            // `px` is Identifier.
            // Parser `parsePrimary`: `match(Number)` -> consumes `1`.
            // Then loop: `peek()` is `Identifier` (`px`).
            // `value += " " + advance().value` -> "1 px".
            // Space added!
            // "1 px solid black".
            // CSS expects "1px". Space breaks it? "border: 1 px solid black" might be invalid CSS or treated differently.
            // "1px" vs "1 px".
            // In CSS, "1 px" is invalid length.
            // The Parser loop adds a space.
            // We need to NOT add space if previous token was Number and next is Identifier/Percent?
            // Or if they are adjacent in source?
            // The tokens track line/column. We can check adjacency.
            return "";
        }
    }

    return "";
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

std::string CodeGen::generateHTML(const ComponentNode& root) {
    std::stringstream ss;

    std::string tag = "div";
    std::string cssClass = toCssClass(root.type);

    if (root.type == "App") {
        tag = "body";
    } else if (root.type == "Text") {
        tag = "span";
    }

    ss << "<" << tag << " class=\"" << cssClass << "\"";

    std::string style = generateInlineStyle(root);
    if (!style.empty()) {
        ss << " " << style;
    }

    ss << ">";

    // For Text component
    if (root.type == "Text") {
        for (const auto& prop : root.properties) {
            if (prop.name == "text") {
                // Evaluate and output
                Evaluator eval;
                ss << eval.evaluate(*prop.value);
            }
        }
    }

    for (const auto& child : root.children) {
        ss << generateHTML(*child);
    }

    ss << "</" << tag << ">";

    return ss.str();
}

std::string CodeGen::generateCSS(const ComponentNode& root) {
    return R"(
.nota-app { margin: 0; padding: 0; width: 100%; height: 100%; }
.nota-rect { display: block; position: relative; }
.nota-text { display: block; position: relative; }
.nota-row { display: flex; flex-direction: row; position: relative; }
.nota-col { display: flex; flex-direction: column; position: relative; }
)";
}
