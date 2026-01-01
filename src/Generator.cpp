#include "Generator.h"
#include <stdexcept>
#include <iostream>

// --- Public Method ---

std::string Generator::Generate(const ProgramNode& program) {
    // 清空上一次的输出
    htmlOutput.str("");
    cssOutput.str("");

    // 从根节点开始遍历
    for (const auto& component : program.components) {
        visit(component);
    }

    // 组合最终的 HTML
    std::stringstream finalHtml;
    finalHtml << "<!DOCTYPE html>\n";
    finalHtml << "<html>\n";
    finalHtml << "<head>\n";
    finalHtml << "  <style>\n";
    finalHtml << cssOutput.str();
    finalHtml << "  </style>\n";
    finalHtml << "</head>\n";
    finalHtml << htmlOutput.str(); // body tag is already in here
    finalHtml << "</html>";

    return finalHtml.str();
}

// --- Private Visitor Methods ---

void Generator::visit(const std::shared_ptr<ASTNode>& node) {
    // 根据节点类型分发
    if (auto component = std::dynamic_pointer_cast<ComponentNode>(node)) {
        visitComponent(*component);
    } else if (auto property = std::dynamic_pointer_cast<PropertyNode>(node)) {
        visitProperty(*property);
    }
}

void Generator::visitComponent(const ComponentNode& component) {
    // 1. 生成唯一的 CSS 类名
    std::string className = generateCssClass(component);
    std::string oldClassName = currentClassName;
    currentClassName = className;

    // 2. 开始生成 HTML 标签
    htmlOutput << generateComponentTag(component) << "\n";

    // 3. 开始生成 CSS 规则
    cssOutput << "." << className << " {\n";
    // 添加所有组件的默认属性
    cssOutput << "  box-sizing: border-box;\n";
    cssOutput << "  overflow: hidden;\n";
    cssOutput << "  margin: 0;\n";
    cssOutput << "  padding: 0;\n";

    // 添加特定组件的默认样式
    if (component.type == "Row") {
        cssOutput << "  display: flex;\n";
        cssOutput << "  flex-direction: row;\n";
    } else if (component.type == "Col") {
        cssOutput << "  display: flex;\n";
        cssOutput << "  flex-direction: column;\n";
    } else if (component.type == "Rect" || component.type == "Text") {
        cssOutput << "  display: block;\n";
    }

    // 4. 遍历子节点（属性和子组件），这会填充 CSS 属性
    for (const auto& child : component.children) {
        // 如果子节点是属性，我们现在就处理它
        if (auto property = std::dynamic_pointer_cast<PropertyNode>(child)) {
             visitProperty(*property);
        }
    }

    // 5. 闭合 CSS 规则
    cssOutput << "}\n\n";

    // 6. 现在单独遍历子组件，生成它们的 HTML
    for (const auto& child : component.children) {
        if (auto child_component = std::dynamic_pointer_cast<ComponentNode>(child)) {
            visitComponent(*child_component);
        }
    }

    // 7. 闭合 HTML 标签
    if (component.type == "App") {
         htmlOutput << "</body>\n";
    } else if (component.type == "Row" || component.type == "Col" || component.type == "Rect") {
         htmlOutput << "</div>\n";
    } else if (component.type == "Text") {
        htmlOutput << "</span>\n";
    }

    // 恢复上一个 class name
    currentClassName = oldClassName;
}

void Generator::visitProperty(const PropertyNode& property) {
    std::string cssProp = getCssPropertyName(property.name);
    std::string cssVal = getCssValue(property.value);

    if (!cssProp.empty() && !cssVal.empty()) {
        cssOutput << "  " << cssProp << ": " << cssVal << ";\n";
    }
}

// --- Private Helper Methods ---

std::string Generator::generateComponentTag(const ComponentNode& component) {
    std::string tag;
    if (component.type == "App")        tag = "<body class=\"nota-app " + currentClassName + "\">";
    else if (component.type == "Row")   tag = "<div class=\"nota-row " + currentClassName + "\">";
    else if (component.type == "Col")   tag = "<div class=\"nota-col " + currentClassName + "\">";
    else if (component.type == "Rect")  tag = "<div class=\"nota-rect " + currentClassName + "\">";
    else if (component.type == "Text")  tag = "<span class=\"nota-text " + currentClassName + "\">";
    else                                tag = "<div class=\"" + currentClassName + "\">"; // 自定义组件默认为 div
    return tag;
}

std::string Generator::generateCssClass(const ComponentNode& component) {
    // 使用组件的内存地址来生成一个“唯一”的哈希值作为类名的一部分
    // 注意：这在多线程或分布式环境中不是一个好主意，但对于单线程编译器 MVP 来说足够了
    static int counter = 0;
    return "nota-" + std::to_string(counter++);
}

std::string Generator::getCssPropertyName(const std::string& notaName) {
    if (notaName == "color") return "background-color";
    if (notaName == "width") return "width";
    if (notaName == "height") return "height";
    if (notaName == "spacing") return "gap";
    if (notaName == "padding") return "padding";
    if (notaName == "border") return "border";
    if (notaName == "radius") return "border-radius";
    // text 属性比较特殊，通常会直接插入到 HTML 中，而不是作为 CSS
    return "";
}

std::string Generator::getCssValue(const PropertyValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return std::to_string(arg) + "px";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        }
        return "";
    }, value);
}
