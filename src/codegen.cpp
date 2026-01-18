#include "codegen.h"

std::string CodeGen::generate(ProgramNode& root) {
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
    // We expect at least one main component (e.g. App)
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void CodeGen::visit(ImportNode& node) {
    // For MVP, imports are ignored in output,
    // maybe later they pull in styles or JS.
    // std::cout << "<!-- Import: " << node.path << " -->\n";
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
    if (name == "color") return "background-color"; // Simplify: color -> bg color for Rect/App?
    // Nota.md says "color => 背景颜色"
    // But for Text? Text { color: ... } usually means text color.
    // For MVP, let's stick to background-color unless we detect Text component context?
    // Hard to pass context down in this simple visitor.
    // Let's assume background-color for now, except maybe special handling later.
    return name;
}

void CodeGen::visit(ComponentNode& node) {
    std::string tag = mapComponentToTag(node.type);

    // For Body (App), we don't indent as it's root-ish, but let's be consistent
    // Actually App -> <body>.
    // If it's App, we assume it closes </head> before it?
    // Ah, my generate() function prints <head>...</head> then calls visitor.
    // But <head> is already closed.
    // So if we visit App, we print <body>...</body>.

    // NOTE: ProgramNode iteration might visit multiple components.
    // Only one should be body. But for now assuming single App.

    indent(html);
    html << "<" << tag;

    // Add classes based on type
    html << " class=\"nota-" << node.type << "\"";

    // Collect properties for inline styles
    std::stringstream styleStream;
    // We need to visit children. Some are properties, some are components.
    // We need to separate them.

    std::vector<std::shared_ptr<ComponentNode>> childrenComponents;

    // Map for flex styles if Row/Col
    if (node.type == "Row") styleStream << "display: flex; flex-direction: row; ";
    if (node.type == "Col") styleStream << "display: flex; flex-direction: column; ";
    if (node.type == "Rect") styleStream << "display: block; ";
    if (node.type == "Text") styleStream << "display: block; ";
    if (node.type == "App") styleStream << "display: block; ";

    std::string innerText = "";

    for (auto& child : node.children) {
        if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
            // It's a property. Visit it to extract value?
            // Visitor pattern on property usually prints.
            // But here we need to capture output to put in style="" attribute.
            // This suggests the visitor might need state or we manually handle properties here.

            // Let's manually handle PropertyNode here for simplicity of styling
            // OR change PropertyNode::accept to write to 'css' stream?
            // But we need it in style="" attribute.

            // Special case: Text content
            if (node.type == "Text" && prop->name == "text") {
                 // The value is the text content
                 if (auto val = std::dynamic_pointer_cast<ValueNode>(prop->value)) {
                     // remove quotes
                     std::string s = val->token.value;
                     if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
                         s = s.substr(1, s.size()-2);
                     }
                     innerText = s;
                 }
                 continue;
            }

            // Special case: id -> id attribute
            if (prop->name == "id") {
                 if (auto val = std::dynamic_pointer_cast<ValueNode>(prop->value)) {
                     html << " id=\"" << val->token.value << "\"";
                 }
                 continue;
            }

            std::string cssName = mapPropertyToCSS(prop->name);

            // Special handling for Text color vs Background color
            if (node.type == "Text" && prop->name == "color") {
                cssName = "color";
            }

            styleStream << cssName << ": ";
            // Get value string
            if (auto val = std::dynamic_pointer_cast<ValueNode>(prop->value)) {
                std::string v = val->token.value;
                // If number (and not color/string), append px if needed?
                // Nota.md: "width, height => 支持数字(px，不带单位的数字都是px)"
                if (val->token.type == TokenType::NUMBER_LITERAL) {
                    // Check if it has %, otherwise add px
                    if (v.find('%') == std::string::npos) {
                        // It's just a number
                         // Wait, Spacing/Gap needs px too.
                         // Opacity/Z-Index don't.
                         // Simple heuristic: width, height, padding, gap/spacing, radius, left, top, right, bottom -> px
                         if (prop->name == "width" || prop->name == "height" ||
                             prop->name == "padding" || prop->name == "spacing" ||
                             prop->name == "radius" || prop->name == "x" || prop->name == "y") {
                             v += "px";
                         }
                    }
                }
                styleStream << v;
            }
            styleStream << "; ";

        } else if (auto comp = std::dynamic_pointer_cast<ComponentNode>(child)) {
            childrenComponents.push_back(comp);
        }
    }

    if (styleStream.str().length() > 0) {
        html << " style=\"" << styleStream.str() << "\"";
    }

    html << ">";

    if (!innerText.empty()) {
        html << innerText;
    }

    if (!childrenComponents.empty()) {
        html << "\n";
        indentLevel++;
        for (auto& comp : childrenComponents) {
            comp->accept(*this);
        }
        indentLevel--;
        indent(html);
    }

    html << "</" << tag << ">\n";
}

void CodeGen::visit(PropertyNode& node) {
    // Not used directly in this approach (handled in Component loop)
}

void CodeGen::visit(ValueNode& node) {
    // Not used directly
}
