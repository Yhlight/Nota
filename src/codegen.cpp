#include "codegen.h"
#include "utils.h"
#include <unordered_map>
#include <vector>
#include <memory>

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
    bool allowUnits = true;   // State to control unit appending

    void visit(ProgramNode&) override {}
    void visit(ImportNode&) override {}
    void visit(ComponentNode&) override {}
    void visit(PropertyNode&) override {}

    void visit(LiteralNode& node) override {
        std::string v = node.token.value;
        if (node.token.type == TokenType::NUMBER_LITERAL) {
            if (allowUnits && v.find('%') == std::string::npos) {
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
        ss << node.name;
    }

    void visit(BinaryExpressionNode& node) override {
        ss << "calc(";
        node.left->accept(*this); // Left operand keeps current unit mode

        ss << " " << node.op.value << " ";

        // If operation is * or /, right operand usually scalar (no units)
        // e.g. 100px * 2. 100px / 2.
        // But 2 * 100px is also valid? My heuristic assumes left is dimension.
        // For simplicity: if * or /, disable units for right side.
        // This fixes `calc(100px * 2px)` -> `calc(100px * 2)`.

        bool prevUnits = allowUnits;
        if (node.op.type == TokenType::STAR || node.op.type == TokenType::SLASH) {
            allowUnits = false;
        }
        node.right->accept(*this);
        allowUnits = prevUnits;

        ss << ")";
    }
};

std::string CodeGen::evaluateExpression(ASTNode& node) {
    return ""; // Placeholder, logic moved to generateStyleAttribute
}

void CodeGen::generateStyleAttribute(const std::vector<std::shared_ptr<ASTNode>>& properties, const std::vector<std::shared_ptr<ASTNode>>& overrideProperties) {
    std::stringstream styleStream;
    std::unordered_map<std::string, std::string> styleMap;

    auto processProps = [&](const std::vector<std::shared_ptr<ASTNode>>& props) {
        for (auto& child : props) {
            if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
                if (prop->name == "text" || prop->name == "id") continue;
                if (prop->name == "onClick" || prop->name == "onHover") continue; // Skip events in styles

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
        if (styleMap.find("position") == styleMap.end()) {
             styleStream << "position: absolute; ";
        }
    } else {
        styleStream << "position: relative; ";
    }

    if (styleStream.str().length() > 0) {
        html << " style=\"" << styleStream.str() << "\"";
    }
}

void CodeGen::generateEvents(const std::vector<std::shared_ptr<ASTNode>>& properties, const std::vector<std::shared_ptr<ASTNode>>& overrideProperties) {
    std::unordered_map<std::string, std::string> eventMap;

    auto processProps = [&](const std::vector<std::shared_ptr<ASTNode>>& props) {
        for (auto& child : props) {
            if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
                std::string attr;
                if (prop->name == "onClick") attr = "onclick";
                else if (prop->name == "onHover") attr = "onmouseenter";

                if (!attr.empty()) {
                     ExpressionStringVisitor eval;
                     prop->value->accept(eval);
                     std::string code = eval.ss.str();

                     // Strip quotes for string literals
                     if (code.size() >= 2 && code.front() == '"' && code.back() == '"') {
                         code = code.substr(1, code.size()-2);
                     }
                     // For code blocks, Parser produces raw string in literal, likely no quotes.

                     eventMap[attr] = code;
                }
            }
        }
    };

    processProps(properties);
    processProps(overrideProperties);

    for (const auto& pair : eventMap) {
        html << " " << pair.first << "=\"" << Utils::escapeHTML(pair.second) << "\"";
    }
}

void CodeGen::visit(ComponentNode& node) {
    // Default entry point visits with empty overrides
    std::unordered_map<std::string, std::vector<std::shared_ptr<ASTNode>>> empty;
    visitComponent(node, empty);
}

void CodeGen::visitComponent(ComponentNode& node, const std::unordered_map<std::string, std::vector<std::shared_ptr<ASTNode>>>& pendingOverrides) {
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

    std::string myId = "";
    // Handle ID (Instance wins)
    for (auto& child : node.children) {
        if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
             if (prop->name == "id") {
                 ExpressionStringVisitor eval;
                 prop->value->accept(eval);
                 myId = eval.ss.str();
                 html << " id=\"" << Utils::escapeHTML(myId) << "\"";
             }
        }
    }
    // If definition has ID (less likely to be useful as it conflicts on reuse, but legal)
    if (myId.empty() && definition) {
        for (auto& child : definition->children) {
            if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
                 if (prop->name == "id") {
                     ExpressionStringVisitor eval;
                     prop->value->accept(eval);
                     myId = eval.ss.str();
                     html << " id=\"" << Utils::escapeHTML(myId) << "\"";
                 }
            }
        }
    }

    // Styles & Events
    std::vector<std::shared_ptr<ASTNode>> defProps;
    if (definition) defProps = definition->children;

    // Check if we have overrides for THIS component (via ID) passed from parent
    std::vector<std::shared_ptr<ASTNode>> parentOverrides;
    if (!myId.empty()) {
        auto it = pendingOverrides.find(myId);
        if (it != pendingOverrides.end()) {
            parentOverrides = it->second;
        }
    }

    // Collect "Deep Overrides" declared IN THIS instance for children
    std::unordered_map<std::string, std::vector<std::shared_ptr<ASTNode>>> childOverrides;

    // Current node overrides (instance props)
    std::vector<std::shared_ptr<ASTNode>> instanceProps;

    for (auto& child : node.children) {
        if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
            // Check for dot notation (deep override)
            size_t dotPos = prop->name.find('.');
            if (dotPos != std::string::npos) {
                // It's a deep override: "rect1.color"
                std::string targetId = prop->name.substr(0, dotPos);
                std::string targetProp = prop->name.substr(dotPos + 1);

                // Create a new PropertyNode for the target
                auto newProp = std::make_shared<PropertyNode>(targetProp, prop->value);
                childOverrides[targetId].push_back(newProp);
            } else {
                instanceProps.push_back(child);
            }
        }
    }

    instanceProps.insert(instanceProps.end(), parentOverrides.begin(), parentOverrides.end());

    generateStyleAttribute(defProps, instanceProps);
    generateEvents(defProps, instanceProps);

    html << ">";

    std::string innerText = "";
    // Helper to extract text
    auto findText = [&](const std::vector<std::shared_ptr<ASTNode>>& props) {
        for (auto& child : props) {
            if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
                 if (node.type == "Text" && prop->name == "text") {
                     ExpressionStringVisitor eval;
                     prop->value->accept(eval);
                     std::string s = eval.ss.str();
                     if (s.size() >= 2 && s.front() == '"' && s.back() == '"') s = s.substr(1, s.size()-2);
                     return s;
                 }
            }
        }
        return std::string("");
    };

    std::string t = findText(instanceProps);
    if (!t.empty()) innerText = t;
    else if (definition) {
        t = findText(defProps);
        if (!t.empty()) innerText = t;
    }

    if (!innerText.empty()) {
        html << innerText;
    }

    if (definition) {
        html << "\n";
        indentLevel++;
        for (auto& child : definition->children) {
            if (auto comp = std::dynamic_pointer_cast<ComponentNode>(child)) {
                visitComponent(*comp, childOverrides); // Pass down
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
                visitComponent(*comp, childOverrides);
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
