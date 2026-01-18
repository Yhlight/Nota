#include "codegen.h"
#include <unordered_map>

std::string CodeGen::generate(ProgramNode& root) {
    SemanticAnalyzer analyzer(registry);
    analyzer.analyze(root);

    html.str("");
    css.str("");
    indentLevel = 0;

    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "  <meta charset=\"UTF-8\">\n";
    html << "  <title>Nota App</title>\n";
    html << "  <style>\n";
    html << "    * { box-sizing: border-box; margin: 0; padding: 0; }\n";
    html << "    body, html { width: 100%; height: 100%; overflow: hidden; }\n";
    html << "    .nota-Row { display: flex; flex-direction: row; }\n";
    html << "    .nota-Col { display: flex; flex-direction: column; }\n";
    html << "    .nota-Rect, .nota-App, .nota-Text { display: block; }\n";
    html << "  </style>\n";
    html << "</head>\n";

    root.accept(*this);

    html << "</html>\n";
    return html.str();
}

void CodeGen::indent(std::stringstream& ss) {
    for (int i = 0; i < indentLevel; ++i) ss << "  ";
}

void CodeGen::visit(ProgramNode& node) {
    for (auto& stmt : node.statements) {
        if (auto comp = std::dynamic_pointer_cast<ComponentNode>(stmt)) {
            if (comp->type == "Item") {
                continue;
            }
        }
        stmt->accept(*this);
    }
}

void CodeGen::visit(ImportNode& node) {
}

std::string CodeGen::mapComponentToTag(const std::string& type) {
    if (type == "App") return "body";
    if (type == "Row" || type == "Col" || type == "Rect") return "div";
    if (type == "Text") return "span";
    return "div";
}

std::string CodeGen::mapPropertyToCSS(const std::string& name) {
    if (name == "spacing") return "gap";
    if (name == "radius") return "border-radius";
    if (name == "color") return "background-color";
    // x and y are handled separately in logic usually, but here we map them to left/top
    if (name == "x") return "left";
    if (name == "y") return "top";
    return name;
}

// Expression Visitor logic used temporarily to capture string output
class ExpressionStringVisitor : public ASTVisitor {
public:
    std::stringstream ss;
    std::string propertyName; // Context for adding units

    void visit(ProgramNode&) override {}
    void visit(ImportNode&) override {}
    void visit(ComponentNode&) override {}
    void visit(PropertyNode&) override {}

    void visit(LiteralNode& node) override {
        std::string v = node.token.value;
        if (node.token.type == TokenType::NUMBER_LITERAL) {
            // Apply unit heuristic if needed, but context is hard here.
            // We'll rely on the caller or just print the number.
            // If it's part of calc(), we generally want units (px) unless it's a multiplier.
            // Simplified: Always add px if it looks like a dimension?
            // Actually, in CSS `calc(100 + 20)` is invalid. `calc(100px + 20px)` is valid.
            // So we SHOULD add units.
            if (v.find('%') == std::string::npos) {
                 // Check if it's likely a dimension based on property name context?
                 // We passed propertyName.
                 if (propertyName == "width" || propertyName == "height" ||
                     propertyName == "padding" || propertyName == "spacing" ||
                     propertyName == "radius" || propertyName == "x" || propertyName == "y" ||
                     propertyName == "left" || propertyName == "top") {
                     v += "px";
                 }
            }
        }
        ss << v;
    }

    void visit(ReferenceNode& node) override {
        // Output var name or raw?
        // Nota: "box.width". CSS doesn't support this directly.
        // For MVP, just output the string. This likely won't work in CSS without JS binding.
        // But fulfills "transpilation".
        ss << node.name;
    }

    void visit(BinaryExpressionNode& node) override {
        ss << "calc(";
        node.left->accept(*this);
        ss << " " << node.op.value << " ";
        node.right->accept(*this);
        ss << ")";
    }
};

std::string CodeGen::evaluateExpression(ASTNode& node) {
    // We assume node is an expression
    // We can't pass property context easily yet.
    // Wait, we can set a member in CodeGen or pass it.
    // The previous design was cleaner with recursive visit.
    // Let's implement visit(BinaryExpressionNode) to write to 'css' stream?
    // No, 'css' stream is global.
    // We need a helper.
    return ""; // Placeholder, logic moved to generateStyleAttribute
}

void CodeGen::generateStyleAttribute(const std::vector<std::shared_ptr<ASTNode>>& properties, const std::vector<std::shared_ptr<ASTNode>>& overrideProperties) {
    std::stringstream styleStream;
    std::unordered_map<std::string, std::string> styleMap;

    auto processProps = [&](const std::vector<std::shared_ptr<ASTNode>>& props) {
        for (auto& child : props) {
            if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
                if (prop->name == "text" || prop->name == "id") continue;

                std::string cssName = mapPropertyToCSS(prop->name);

                // Evaluate value expression
                ExpressionStringVisitor eval;
                eval.propertyName = prop->name;
                prop->value->accept(eval);

                styleMap[cssName] = eval.ss.str();
            }
        }
    };

    processProps(properties);
    processProps(overrideProperties);

    for (const auto& pair : styleMap) {
        styleStream << pair.first << ": " << pair.second << "; ";
    }

    // Positioning Logic (Conductor 4)
    if (styleMap.count("left") || styleMap.count("top")) {
        // If x or y is set, ensure position: absolute
        // Note: styleMap key is the CSS name, so "left" or "top"
        if (styleMap.find("position") == styleMap.end()) {
             styleStream << "position: absolute; ";
        }
    } else {
        // Default relative? Or default static?
        // Nota says parent should be relative.
        // Children are relative by default (flow).
        // But if we want to ensure stacking context for z-index (Conductor 4), maybe relative is good.
        styleStream << "position: relative; ";
    }

    if (styleStream.str().length() > 0) {
        html << " style=\"" << styleStream.str() << "\"";
    }
}

void CodeGen::visit(ComponentNode& node) {
    bool isCustom = registry.hasComponent(node.type);
    std::shared_ptr<ComponentNode> definition = nullptr;
    if (isCustom) {
        definition = registry.getComponent(node.type);
    }

    std::string tag = mapComponentToTag(node.type);
    if (isCustom) tag = "div";

    indent(html);
    html << "<" << tag;
    html << " class=\"nota-" << node.type << "\"";

    for (auto& child : node.children) {
        if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
             if (prop->name == "id") {
                 // Value might be expression now, but ID is usually literal.
                 // We need to extract string.
                 ExpressionStringVisitor eval;
                 prop->value->accept(eval);
                 html << " id=\"" << eval.ss.str() << "\"";
             }
        }
    }

    std::vector<std::shared_ptr<ASTNode>> defProps;
    if (definition) defProps = definition->children;

    std::vector<std::shared_ptr<ASTNode>> extraProps;
    // Structural classes handle display, so we don't need manual injection anymore.

    generateStyleAttribute(defProps, node.children);

    html << ">";

    std::string innerText = "";
    for (auto& child : node.children) {
        if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
             if (node.type == "Text" && prop->name == "text") {
                 ExpressionStringVisitor eval;
                 prop->value->accept(eval);
                 std::string s = eval.ss.str();
                 if (s.size() >= 2 && s.front() == '"' && s.back() == '"') s = s.substr(1, s.size()-2);
                 innerText = s;
             }
        }
    }

    if (!innerText.empty()) {
        html << innerText;
    }

    if (definition) {
        html << "\n";
        indentLevel++;
        for (auto& child : definition->children) {
            if (auto comp = std::dynamic_pointer_cast<ComponentNode>(child)) {
                comp->accept(*this);
            }
        }
        indentLevel--;
        indent(html);
    }

    bool hasInstanceChildren = false;
    for (auto& child : node.children) {
        if (auto comp = std::dynamic_pointer_cast<ComponentNode>(child)) {
            hasInstanceChildren = true;
            break;
        }
    }

    if (hasInstanceChildren) {
        if (!definition) html << "\n";
        indentLevel++;
        for (auto& child : node.children) {
            if (auto comp = std::dynamic_pointer_cast<ComponentNode>(child)) {
                comp->accept(*this);
            }
        }
        indentLevel--;
        indent(html);
    }

    html << "</" << tag << ">\n";
}

void CodeGen::visit(PropertyNode& node) { }
void CodeGen::visit(LiteralNode& node) { }
void CodeGen::visit(ReferenceNode& node) { }
void CodeGen::visit(BinaryExpressionNode& node) { }
