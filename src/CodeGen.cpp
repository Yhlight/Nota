#include "CodeGen.h"
#include <sstream>
#include <algorithm>
#include <vector>

// Helper to convert Nota type to CSS class
static std::string toCssClass(const std::string& type) {
    std::string lowerType = type;
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    return "nota-" + lowerType;
}

static std::string generateInlineStyle(const ComponentNode& node) {
    std::stringstream ss;
    bool hasStyle = false;

    for (const auto& prop : node.properties) {
        if (prop.name == "text") continue; // handled in content

        // Skip id for style
        if (prop.name == "id") continue;

        if (!hasStyle) {
            ss << "style=\"";
            hasStyle = true;
        }

        std::string cssName = prop.name;
        std::string value = prop.value;

        // Positioning logic
        if (cssName == "x") {
            cssName = "left";
            // Ensure position: absolute is added once
            if (!hasStyle || ss.str().find("position: absolute") == std::string::npos) {
                ss << "position: absolute; ";
            }
        } else if (cssName == "y") {
            cssName = "top";
            if (!hasStyle || ss.str().find("position: absolute") == std::string::npos) {
                ss << "position: absolute; ";
            }
        } else if (cssName == "position") {
            // Nota "position" keyword handling
            // position: left top, position: center, etc.
            // Values might be unquoted identifiers.

            // "center" -> left: 50%; top: 50%; transform: translate(-50%, -50%); position: absolute;
            if (value == "center") {
                ss << "left: 50%; top: 50%; transform: translate(-50%, -50%); position: absolute; ";
                continue;
            } else if (value == "left top" || value == "top left") {
                ss << "left: 0; top: 0; position: absolute; ";
                continue;
            }
            // TODO: Implement other positions (right top, right bottom, etc.)

            // If unknown, fallback (or should we?)
            continue;
        }

        // Simple mapping
        if (cssName == "color") cssName = "background-color";
        if (cssName == "radius") cssName = "border-radius";
        if (cssName == "spacing") cssName = "gap";

        // Handle units
        // If value is number (digits, maybe one decimal point), append px.
        // If string or ends in % or other unit, leave it.
        bool isNumber = !value.empty() && std::all_of(value.begin(), value.end(), [](char c){
            return isdigit(c) || c == '.';
        });

        // Simple check: if it looks like a number and doesn't already have a unit (like %)
        if (isNumber && value.find('.') != std::string::npos) {
             // It's a float, definitely needs unit if it's a length.
             // But for now we assume all unitless numbers are px
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

    // For Text component, check if there is a 'text' property and output it
    if (root.type == "Text") {
        for (const auto& prop : root.properties) {
            if (prop.name == "text") {
                ss << prop.value;
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
    // Return base styles for the supported components
    return R"(
.nota-app { margin: 0; padding: 0; width: 100%; height: 100%; }
.nota-rect { display: block; position: relative; }
.nota-text { display: block; position: relative; }
.nota-row { display: flex; flex-direction: row; position: relative; }
.nota-col { display: flex; flex-direction: column; position: relative; }
)";
}
