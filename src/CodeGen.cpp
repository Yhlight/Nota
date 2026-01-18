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

        // Simple mapping
        if (cssName == "color") cssName = "background-color";
        if (cssName == "radius") cssName = "border-radius";

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
.nota-rect { display: block; }
.nota-text { display: block; }
.nota-row { display: flex; flex-direction: row; }
.nota-col { display: flex; flex-direction: column; }
)";
}
