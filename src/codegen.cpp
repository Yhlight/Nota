#include "codegen.h"
#include "utils.h"
#include "runtime.h"
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

    js.str("");
    js << NOTA_RUNTIME_JS;

    root.accept(*this);

    html << js.str();
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

void CodeGen::visit(ImportNode& node) {}
void CodeGen::visit(PackageNode& node) {}

std::string CodeGen::mapComponentToTag(const std::string& type) {
    if (type == "App") return "body";
    if (type == "Row" || type == "Col" || type == "Rect") return "div";
    if (type == "Text") return "span";
    return "div";
}

std::string CodeGen::mapPropertyToCSS(const std::string& name, bool isTextComponent) {
    if (name == "width") return "width";
    if (name == "height") return "height";
    if (name == "spacing") return "gap";
    if (name == "padding") return "padding";
    if (name == "radius") return "border-radius";
    if (name == "index") return "z-index";
    if (name == "color") {
        if (isTextComponent) return "color";
        return "background-color";
    }
    // x and y are handled separately in logic usually, but here we map them to left/top
    if (name == "x") return "left";
    if (name == "y") return "top";

    // Whitelist CSS Layout Props (Simple check)
    if (name == "left" || name == "top" || name == "right" || name == "bottom" || name == "position") return name;
    if (name == "display" || name == "opacity" || name == "visible") return name;
    if (name == "border") return "border";

    // Everything else is considered a custom property (attribute)
    return "";
}

// Expression Visitor logic used temporarily to capture string output
class ExpressionStringVisitor : public ASTVisitor {
public:
    std::stringstream ss;
    std::string propertyName; // Context for adding units
    bool allowUnits = true;   // State to control unit appending
    const std::set<std::string>* activeIterators = nullptr;

    void visit(ProgramNode&) override {}
    void visit(ImportNode&) override {}
    void visit(PackageNode&) override {}
    void visit(ConditionalNode&) override {}
    void visit(PropertyNode&) override {}

    void visit(LiteralNode& node) override {
        std::string v = node.token.value;
        bool isNumber = (node.token.type == TokenType::NUMBER_LITERAL);

        // Parser creates STRING_LITERAL even for numbers if they are single tokens in parsePrimary
        if (!isNumber && node.token.type == TokenType::STRING_LITERAL) {
            // Check if pure number
            if (!v.empty() && v.find_first_not_of("0123456789.-") == std::string::npos) {
                 // Check for negative number or just dots?
                 // Simple check: digit or dot or minus
                 isNumber = true;
            }
        }

        if (isNumber) {
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
        // Check Layout Parent References: parent.width / parent.height -> 100%
        if (node.name == "parent.width" || node.name == "parent.height") {
            ss << "100%";
            return;
        }

        // Check if reference is an active iterator or property of one (item or item.prop)
        if (activeIterators) {
            std::string root = node.name;
            size_t dotPos = root.find('.');
            size_t bracketPos = root.find('[');

            size_t endPos = std::string::npos;
            if (dotPos != std::string::npos) endPos = dotPos;
            if (bracketPos != std::string::npos && bracketPos < endPos) endPos = bracketPos;

            if (endPos != std::string::npos) {
                root = root.substr(0, endPos);
            }

            if (activeIterators->count(root)) {
                // Interpolate: item.name -> ${item.name}
                // item[0] -> ${item[0]}
                ss << "${" << node.name << "}";
                return;
            }
        }
        ss << node.name;
    }

    void visit(BinaryExpressionNode& node) override {
        ss << "calc(";
        node.left->accept(*this); // Left operand keeps current unit mode

        ss << " " << node.op.value << " ";

        bool prevUnits = allowUnits;
        if (node.op.type == TokenType::STAR || node.op.type == TokenType::SLASH) {
            allowUnits = false;
        }
        node.right->accept(*this);
        allowUnits = prevUnits;

        ss << ")";
    }

    void visit(StructDefinitionNode& node) override {}

    void visit(StructInstantiationNode& node) override {
        // Output as JS instantiation: new Name(args)
        ss << "new " << node.structName << "(";
        for (size_t i = 0; i < node.arguments.size(); ++i) {
            if (i > 0) ss << ", ";
            node.arguments[i]->accept(*this);
        }
        ss << ")";
    }

    void visit(ListNode& node) override {
        ss << "[";
        for (size_t i = 0; i < node.elements.size(); ++i) {
            if (i > 0) ss << ", ";
            node.elements[i]->accept(*this);
        }
        ss << "]";
    }

    void visit(DelegateNode& node) override {}
    void visit(ForNode& node) override {}

    void visit(ComponentNode& node) override {
        // Generates JS Object Literal: { type: "Name", prop: val, ... }
        ss << "{ type: \"" << node.type << "\"";
        if (!node.name.empty()) {
            ss << ", name: \"" << node.name << "\"";
        }

        for (auto& child : node.children) {
            if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
                ss << ", " << prop->name << ": ";
                prop->value->accept(*this);
            }
            // Nested components in expressions? e.g. State { State { ... } }
            // If child is component, it's likely not a property but a child?
            // For Object Literals, usually everything is a property.
            // But if we support children in State, we need a children array?
            // Nota.md doesn't specify. Assuming properties for now.
        }
        ss << " }";
    }
};

std::string CodeGen::evaluateExpression(ASTNode& node) {
    return ""; // Placeholder, logic moved to generateStyleAttribute
}

void CodeGen::generateStyleAttribute(const std::vector<std::shared_ptr<ASTNode>>& properties, const std::vector<std::shared_ptr<ASTNode>>& overrideProperties, bool isTextComponent) {
    std::stringstream styleStream;
    std::unordered_map<std::string, std::string> styleMap;
    std::unordered_map<std::string, std::string> attrMap;

    auto processProps = [&](const std::vector<std::shared_ptr<ASTNode>>& props) {
        for (auto& child : props) {
            if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
                std::string propName = prop->name;
                // Handle typed properties: "property type name"
                if (propName.rfind("property ", 0) == 0) {
                    size_t lastSpace = propName.rfind(' ');
                    if (lastSpace != std::string::npos) {
                        propName = propName.substr(lastSpace + 1);
                    }
                }

                if (propName == "text" || propName == "id" || propName == "states") continue;
                if (propName == "onClick" || propName == "onHover") continue; // Skip events in styles

                std::string cssName = mapPropertyToCSS(propName, isTextComponent);

                // Evaluate value expression
                ExpressionStringVisitor eval;
                eval.propertyName = propName;
                eval.activeIterators = &activeIterators;
                if (prop->value) {
                    prop->value->accept(eval);
                    if (!cssName.empty()) {
                        styleMap[cssName] = eval.ss.str();
                    } else {
                        // Custom property -> Data Attribute
                        attrMap["data-" + propName] = eval.ss.str();
                    }
                }
            }
        }
    };

    processProps(properties);
    processProps(overrideProperties);

    // Positioning Logic (Conductor 4)
    if (styleMap.count("left") || styleMap.count("top")) {
        // If position anchor is set, adjust mapping?
        bool hasPosition = styleMap.count("position");
        if (hasPosition) {
            std::string posVal = styleMap["position"];
            bool isRight = (posVal.find("right") != std::string::npos);
            bool isBottom = (posVal.find("bottom") != std::string::npos);
            bool isCenter = (posVal.find("center") != std::string::npos);

            if (isRight) {
                if (styleMap.count("left")) {
                    styleMap["right"] = styleMap["left"];
                    styleMap.erase("left");
                }
            }

            if (isBottom) {
                if (styleMap.count("top")) {
                    styleMap["bottom"] = styleMap["top"];
                    styleMap.erase("top");
                }
            }

            if (isCenter) {
                if (styleMap.count("left")) {
                    styleMap["left"] = "calc(50% + " + styleMap["left"] + ")";
                } else {
                    styleMap["left"] = "50%";
                }

                if (styleMap.count("top")) {
                    styleMap["top"] = "calc(50% + " + styleMap["top"] + ")";
                } else {
                    styleMap["top"] = "50%";
                }
                styleMap["transform"] = "translate(-50%, -50%)";
            }

            styleMap.erase("position");
        }

        if (styleMap.find("position") == styleMap.end()) {
             styleStream << "position: absolute; ";
        }
    } else {
        styleStream << "position: relative; ";
    }

    for (const auto& pair : styleMap) {
        styleStream << pair.first << ": " << pair.second << "; ";
    }

    if (styleStream.str().length() > 0) {
        html << " style=\"" << styleStream.str() << "\"";
    }

    // Output Attributes
    for (const auto& pair : attrMap) {
        html << " " << pair.first << "=\"" << Utils::escapeHTML(pair.second) << "\"";
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
                     eval.activeIterators = &activeIterators;
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

    // 1. Resolve Overrides & Properties first

    // Check if we have overrides for THIS component (via ID) passed from parent
    // Problem: We need ID to find overrides. But ID might be IN overrides (chicken-egg).
    // Assumption: ID is usually defined in the component or definition, not injected via override (though possible).
    // For now, we resolve ID from definition/instance first.

    std::string myId = "";
    // Handle ID (Instance wins)
    for (auto& child : node.children) {
        if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
             if (prop->name == "id") {
                 ExpressionStringVisitor eval;
                 prop->value->accept(eval);
                 myId = eval.ss.str();
                 break;
             }
        }
    }
    // If definition has ID
    if (myId.empty() && definition) {
        for (auto& child : definition->children) {
            if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
                 if (prop->name == "id") {
                     ExpressionStringVisitor eval;
                     prop->value->accept(eval);
                     myId = eval.ss.str();
                     break;
                 }
            }
        }
    }

    std::vector<std::shared_ptr<ASTNode>> parentOverrides;
    if (!myId.empty()) {
        auto it = pendingOverrides.find(myId);
        if (it != pendingOverrides.end()) {
            parentOverrides = it->second;
        }
    }

    std::unordered_map<std::string, std::vector<std::shared_ptr<ASTNode>>> childOverrides;
    std::vector<std::shared_ptr<ASTNode>> instanceProps;

    for (auto& child : node.children) {
        if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
            // Check for dot notation (deep override)
            size_t dotPos = prop->name.find('.');
            if (dotPos != std::string::npos) {
                std::string targetId = prop->name.substr(0, dotPos);
                std::string targetProp = prop->name.substr(dotPos + 1);
                auto newProp = std::make_shared<PropertyNode>(targetProp, prop->value);
                childOverrides[targetId].push_back(newProp);
            } else {
                instanceProps.push_back(child);
            }
        }
    }
    instanceProps.insert(instanceProps.end(), parentOverrides.begin(), parentOverrides.end());

    // 2. Detect States
    bool hasStates = false;
    std::shared_ptr<PropertyNode> statesProp = nullptr;

    auto checkStates = [&](const std::vector<std::shared_ptr<ASTNode>>& nodes) {
        for (auto& child : nodes) {
            if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
                if (prop->name == "states") { hasStates = true; statesProp = prop; break; }
            }
        }
    };
    checkStates(instanceProps);
    if (!hasStates && definition) {
        checkStates(definition->children);
    }

    // 3. Auto-generate ID if needed
    if (myId.empty() && hasStates) {
        static int compCount = 0;
        myId = "nota-comp-" + std::to_string(++compCount);
    }

    // 4. Generate HTML
    indent(html);
    html << "<" << tag;
    html << " class=\"nota-" << node.type << "\"";

    if (!myId.empty()) {
        html << " id=\"" << Utils::escapeHTML(myId) << "\"";
    }

    std::vector<std::shared_ptr<ASTNode>> defProps;
    if (definition) defProps = definition->children;

    bool isText = (node.type == "Text");
    generateStyleAttribute(defProps, instanceProps, isText);
    generateEvents(defProps, instanceProps);

    html << ">";

    std::string innerText = "";
    // Helper to extract text
    auto findText = [&](const std::vector<std::shared_ptr<ASTNode>>& props) {
        for (auto& child : props) {
            if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
                 if (node.type == "Text" && prop->name == "text") {
                     ExpressionStringVisitor eval;
                     eval.activeIterators = &activeIterators;
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

    auto processChildren = [&](const std::vector<std::shared_ptr<ASTNode>>& children, const std::unordered_map<std::string, std::vector<std::shared_ptr<ASTNode>>>& overrides) {
        for (auto& child : children) {
            if (auto cond = std::dynamic_pointer_cast<ConditionalNode>(child)) {
                visit(*cond);
            } else if (auto deleg = std::dynamic_pointer_cast<DelegateNode>(child)) {
                visit(*deleg);
            } else if (auto loop = std::dynamic_pointer_cast<ForNode>(child)) {
                visit(*loop);
            } else if (auto comp = std::dynamic_pointer_cast<ComponentNode>(child)) {
                visitComponent(*comp, overrides);
            }
        }
    };

    if (definition) {
        html << "\n";
        indentLevel++;
        processChildren(definition->children, childOverrides);
        indentLevel--;
        indent(html);
    }

    bool hasInstanceChildren = false;
    for (auto& child : node.children) {
        if (std::dynamic_pointer_cast<ComponentNode>(child) ||
            std::dynamic_pointer_cast<ConditionalNode>(child) ||
            std::dynamic_pointer_cast<DelegateNode>(child) ||
            std::dynamic_pointer_cast<ForNode>(child)) {
            hasInstanceChildren = true;
            break;
        }
    }

    if (hasInstanceChildren) {
        if (!definition) html << "\n";
        indentLevel++;
        processChildren(node.children, childOverrides);
        indentLevel--;
        indent(html);
    }

    html << "</" << tag << ">\n";

    if (hasStates && !myId.empty()) {
        js << "<script>\n";
        js << "nota_elements['" << myId << "'] = new NotaComponent(document.getElementById('" << myId << "'), {\n";
        js << "  states: ";
        ExpressionStringVisitor eval;
        statesProp->value->accept(eval);
        js << eval.ss.str();
        js << "\n});\n";
        js << "</script>\n";
    }
}

void CodeGen::visit(ConditionalNode& node) {
    static int ifCount = 0;
    std::string id = "nota-if-" + std::to_string(++ifCount);

    html << "<div id=\"" << id << "\" style=\"display: contents;\">\n";

    std::string thenId = id + "-then";
    std::string elseId = id + "-else";

    html << "<div id=\"" << thenId << "\">\n";
    indentLevel++;
    for (auto& child : node.thenBranch) {
        child->accept(*this);
    }
    indentLevel--;
    html << "</div>\n";

    if (!node.elseBranch.empty()) {
        html << "<div id=\"" << elseId << "\" style=\"display: none;\">\n";
        indentLevel++;
        for (auto& child : node.elseBranch) {
            child->accept(*this);
        }
        indentLevel--;
        html << "</div>\n";
    }

    html << "</div>\n";

    if (auto ref = std::dynamic_pointer_cast<ReferenceNode>(node.condition)) {
        html << "<script>\n";
        html << "  // Logic for if (" << ref->name << ")\n";
        html << "  // Requires runtime property system.\n";
        html << "</script>\n";
    }
}

void CodeGen::visit(StructDefinitionNode& node) {
    html << "<script>\n";
    html << "class " << node.name << " {\n";
    html << "  constructor(";
    for (size_t i = 0; i < node.fields.size(); ++i) {
        if (i > 0) html << ", ";
        html << node.fields[i].name;
    }
    html << ") {\n";
    for (auto& field : node.fields) {
        html << "    this." << field.name << " = " << field.name << ";\n";
    }
    html << "  }\n";
    html << "}\n";
    html << "</script>\n";
}

void CodeGen::visit(DelegateNode& node) {
    // Note: We use a local stream for inline JS to avoid flushing/polluting the global 'js' stream.
    std::stringstream localJs;
    localJs << "<script>\n";
    localJs << "  (function() {\n";
    localJs << "    const el = document.currentScript.parentElement;\n";
    localJs << "    const targetId = '" << node.target << "';\n";
    localJs << "    let target = null;\n";
    localJs << "    if (targetId === 'parent') target = el.parentElement;\n";
    localJs << "    else target = document.getElementById(targetId);\n";
    localJs << "    if (target) {\n";
    for (const auto& evt : node.events) {
        std::string domEvt = "click";
        if (evt == "onClick") domEvt = "click";
        else if (evt == "onHover") domEvt = "mouseenter";

        localJs << "      el.addEventListener('" << domEvt << "', (e) => {\n";
        localJs << "        // Dispatch event on target\n";
        localJs << "        target.dispatchEvent(new Event('" << domEvt << "'));\n";
        localJs << "      });\n";
    }
    localJs << "    }\n";
    localJs << "  })();\n";
    localJs << "</script>\n";

    html << localJs.str();
}

void CodeGen::visit(ForNode& node) {
    static int forCount = 0;
    std::string containerId = "nota-for-" + std::to_string(++forCount);

    indent(html);
    html << "<div id=\"" << containerId << "\" style=\"display: contents;\"></div>\n";

    std::set<std::string> oldIterators = this->activeIterators;

    CodeGen bodyGen;
    bodyGen.registry = this->registry;
    bodyGen.activeIterators = this->activeIterators;
    bodyGen.activeIterators.insert(node.iterator);

    for (auto& child : node.body) {
        child->accept(bodyGen);
    }

    std::string templateHTML = bodyGen.html.str();

    // Escape backticks in the template string
    std::string escapedTemplate = templateHTML;
    size_t pos = 0;
    while ((pos = escapedTemplate.find('`', pos)) != std::string::npos) {
         escapedTemplate.replace(pos, 1, "\\`");
         pos += 2;
    }

    // Use local stream for script generation
    std::stringstream localJs;
    localJs << "<script>\n";
    localJs << "(function() {\n";
    localJs << "  const container = document.getElementById('" << containerId << "');\n";
    localJs << "  const list = " << node.listName << ";\n";
    localJs << "  const template = ( " << node.iterator << " ) => `" << escapedTemplate << "`;\n";
    localJs << "  let html = '';\n";
    localJs << "  if (list) {\n";
    localJs << "    list.forEach(item => {\n";
    localJs << "      html += template(item);\n";
    localJs << "    });\n";
    localJs << "  }\n";
    localJs << "  container.innerHTML = html;\n";
    localJs << "})();\n";
    localJs << "</script>\n";

    html << localJs.str();

    this->activeIterators = oldIterators;
}

void CodeGen::visit(StructInstantiationNode& node) { }
void CodeGen::visit(ListNode& node) { }
void CodeGen::visit(PropertyNode& node) { }
void CodeGen::visit(LiteralNode& node) { }
void CodeGen::visit(ReferenceNode& node) { }
void CodeGen::visit(BinaryExpressionNode& node) { }
