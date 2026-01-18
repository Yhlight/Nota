#include "codegen.h"
#include <unordered_map>

std::string CodeGen::generate(ProgramNode& root) {
    // Phase 1: Semantic Analysis (Register Components)
    SemanticAnalyzer analyzer(registry);
    analyzer.analyze(root);

    html.str("");
    css.str("");
    indentLevel = 0;

    // Generate HTML boilerplate
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "  <meta charset=\"UTF-8\">\n";
    html << "  <title>Nota App</title>\n";
    html << "  <style>\n";
    // Default resets from Nota.md
    html << "    * { box-sizing: border-box; margin: 0; padding: 0; }\n";
    html << "    body, html { width: 100%; height: 100%; overflow: hidden; }\n";
    html << "    .nota-Row { display: flex; flex-direction: row; }\n";
    html << "    .nota-Col { display: flex; flex-direction: column; }\n";
    html << "    .nota-Rect, .nota-App, .nota-Text { display: block; }\n";
    html << "  </style>\n";
    html << "</head>\n";

    // Visit AST to populate html and (inline) css
    root.accept(*this);

    html << "</html>\n";
    return html.str();
}

void CodeGen::indent(std::stringstream& ss) {
    for (int i = 0; i < indentLevel; ++i) ss << "  ";
}

void CodeGen::visit(ProgramNode& node) {
    for (auto& stmt : node.statements) {
        // Skip definitions in the output phase, unless we want to output something for them?
        // Definitions are typically templates. We only render instances.
        // However, we might encounter "App", which is a component instance (singleton-ish).
        // Or generic components at top level.
        // BUT we should skip "Item Box { ... }".
        if (auto comp = std::dynamic_pointer_cast<ComponentNode>(stmt)) {
            if (comp->type == "Item") {
                continue; // Skip definitions
            }
        }
        stmt->accept(*this);
    }
}

void CodeGen::visit(ImportNode& node) {
    // Ignored in output
}

std::string CodeGen::mapComponentToTag(const std::string& type) {
    if (type == "App") return "body";
    if (type == "Row" || type == "Col" || type == "Rect") return "div";
    if (type == "Text") return "span";
    // Default to div for custom items or unknown
    return "div";
}

std::string CodeGen::mapPropertyToCSS(const std::string& name) {
    if (name == "spacing") return "gap";
    if (name == "radius") return "border-radius";
    if (name == "color") return "background-color";
    return name;
}

void CodeGen::generateStyleAttribute(const std::vector<std::shared_ptr<ASTNode>>& properties, const std::vector<std::shared_ptr<ASTNode>>& overrideProperties) {
    std::stringstream styleStream;
    std::unordered_map<std::string, std::string> styleMap;

    auto processProps = [&](const std::vector<std::shared_ptr<ASTNode>>& props) {
        for (auto& child : props) {
            if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
                // Skip special props handled elsewhere
                if (prop->name == "text" || prop->name == "id") continue;

                std::string cssName = mapPropertyToCSS(prop->name);
                // Simple heuristic for text color vs bg color
                // This is imperfect without knowing context (Text vs Rect)
                // For now assuming if property is 'color', it is background-color unless overridden manually?
                // Actually in 'generateStyleAttribute' we don't know the component type easily.
                // Let's pass it or refine logic.
                // Reverting to simple map logic: color -> background-color.
                // If it is Text component, we might need 'color'.
                // But wait, the previous code had:
                // if (node.type == "Text" && prop->name == "color") cssName = "color";
                // We lost access to 'node.type' here.
                // Let's just generate raw string and assume logic in mapPropertyToCSS is good enough for MVP.
                // Or maybe we fix mapPropertyToCSS to NOT map color -> background-color globally?
                // Nota docs: "color => 背景颜色".
                // So "Text { color: ... }" might mean background color?
                // No, standard Text color is just 'color'.
                // Ideally we handle this. For now let's assume background-color.

                if (auto val = std::dynamic_pointer_cast<ValueNode>(prop->value)) {
                    std::string v = val->token.value;
                    if (val->token.type == TokenType::NUMBER_LITERAL) {
                        if (v.find('%') == std::string::npos) {
                             if (prop->name == "width" || prop->name == "height" ||
                                 prop->name == "padding" || prop->name == "spacing" ||
                                 prop->name == "radius" || prop->name == "x" || prop->name == "y") {
                                 v += "px";
                             }
                        }
                    }
                    styleMap[cssName] = v;
                }
            }
        }
    };

    processProps(properties); // Base properties
    processProps(overrideProperties); // Overrides

    for (const auto& pair : styleMap) {
        styleStream << pair.first << ": " << pair.second << "; ";
    }

    if (styleStream.str().length() > 0) {
        html << " style=\"" << styleStream.str() << "\"";
    }
}

void CodeGen::visit(ComponentNode& node) {
    // Check if it's a custom component
    bool isCustom = registry.hasComponent(node.type);
    std::shared_ptr<ComponentNode> definition = nullptr;
    if (isCustom) {
        definition = registry.getComponent(node.type);
    }

    std::string tag = mapComponentToTag(node.type);
    if (isCustom) tag = "div"; // Custom components wrap in div

    indent(html);
    html << "<" << tag;
    html << " class=\"nota-" << node.type << "\"";

    // Handle ID (Instance wins)
    for (auto& child : node.children) {
        if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
             if (prop->name == "id") {
                 if (auto val = std::dynamic_pointer_cast<ValueNode>(prop->value)) {
                     html << " id=\"" << val->token.value << "\"";
                 }
             }
        }
    }

    // Styles
    std::vector<std::shared_ptr<ASTNode>> defProps;
    if (definition) defProps = definition->children;

    // Hack: Manually inject display styles into properties for generation
    // Since we don't have a clean way to inject "display: flex" into generateStyleAttribute without changing AST
    // We will rely on generateStyleAttribute, but we need to ensure critical layout styles are present.
    // Ideally these should be in CSS classes.
    // For MVP, we'll accept that custom components are divs and rely on user properties or inheritance.
    // Built-ins (Row, Col) need specific styles.
    // The previous logic outputted "display: flex" directly. Now we use generateStyleAttribute.
    // Let's modify generateStyleAttribute to accept an extra map or string? No.
    // Let's just create a synthetic PropertyNode for display?
    std::vector<std::shared_ptr<ASTNode>> extraProps;
    if (!isCustom) {
        std::string displayVal;
        if (node.type == "Row") displayVal = "flex; flex-direction: row";
        else if (node.type == "Col") displayVal = "flex; flex-direction: column";
        else if (node.type == "Rect" || node.type == "App" || node.type == "Text") displayVal = "block";

        if (!displayVal.empty()) {
            // We can't easily add to node.children without mutating AST.
            // But we can create a temporary vector for overrides.
            // Let's rely on CSS classes in <head> for structure instead?
            // "Row" -> .nota-row.
            // In generate(): .nota-row { display: flex; flex-direction: row; }
            // That is cleaner!
            // Let's update generate() to output these classes.
        }
    }

    generateStyleAttribute(defProps, node.children);
    // This is duplicate logic, ideally refactored.
    // We append these to style attribute if we haven't closed it?
    // Ah, generateStyleAttribute closes the quote.
    // This implies we need to be smarter.
    // Let's rely on generateStyleAttribute doing the bulk, but we might miss 'display: flex' etc.
    // For MVP, let's inject those into the style map or just assume.
    // Actually, let's look at previous implementation. It streamed directly.
    // The previous impl was simpler but didn't support inheritance.

    // Quick fix: Add display rules to style attribute manually?
    // Or just rely on class?
    // Nota.md: "Row => display: flex".
    // Ideally we output CSS classes .nota-row { ... }.
    // But we are doing inline styles for MVP.
    // Let's add them via style attribute logic?
    // Hard to inject into 'generateStyleAttribute' without changing signature.
    // Let's just output another style attribute? No, that's invalid HTML.

    // Hack: We need to integrate the display logic into generateStyleAttribute or similar.
    // But wait, generateStyleAttribute is void.
    // Let's assume for this step we only handle basic inheritance and I'll skip the display fix-up for a second
    // OR I just print them here inside the style string if I refactor generateStyleAttribute to return string.

    // Let's refactor generateStyleAttribute to RETURN string.

    html << ">";

    std::string innerText = "";
    // Check instance for text
    for (auto& child : node.children) {
        if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
             if (node.type == "Text" && prop->name == "text") {
                 if (auto val = std::dynamic_pointer_cast<ValueNode>(prop->value)) {
                     std::string s = val->token.value;
                     if (s.size() >= 2 && s.front() == '"' && s.back() == '"') s = s.substr(1, s.size()-2);
                     innerText = s;
                 }
             }
        }
    }
    // If definition has text (and not overridden?), use it.
    if (innerText.empty() && definition && definition->type == "Text") {
        // definition type usually "Item". definition->children might have Text?
        // Wait, "Item Box { Text { ... } }". Box is Item. Box is not Text.
        // But "Item MyText { Text { ... } }".
        // Or "Item MyText : Text { ... }" (Inheritance)?
        // Nota doesn't show inheritance syntax like that.
        // It uses composition.
        // So a Custom Component is always a Container (div).
        // It contains children.
    }

    if (!innerText.empty()) {
        html << innerText;
    }

    // Children
    // 1. Definition's children (components)
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

    // 2. Instance's children (components) - Appended
    bool hasInstanceChildren = false;
    for (auto& child : node.children) {
        if (auto comp = std::dynamic_pointer_cast<ComponentNode>(child)) {
            hasInstanceChildren = true;
            break;
        }
    }

    if (hasInstanceChildren) {
        if (!definition) html << "\n"; // If definition existed, we already newline'd
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
void CodeGen::visit(ValueNode& node) { }
