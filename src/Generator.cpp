#include "Generator.h"
#include <stdexcept>
#include <iostream>

// --- Public Method ---

std::string Generator::Generate(const ProgramNode& program) {
    // 清空状态
    htmlOutput.str("");
    cssOutput.str("");
    componentRegistry.clear();

    // 1. 预处理阶段：扫描并注册所有组件定义
    preprocessDefinitions(program);

    // 2. 生成阶段：只访问顶级的组件实例
    for (const auto& node : program.children) {
        if (auto component = std::dynamic_pointer_cast<ComponentNode>(node)) {
            if (component->type == "App") {
                appComponentType = component->type;
            }
            visit(component);
        }
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
    if (appComponentType == "App") {
        finalHtml << "</body>\n";
    }
    finalHtml << "</html>";

    return finalHtml.str();
}

// --- Private Visitor Methods ---

void Generator::visit(const std::shared_ptr<ASTNode>& node) {
    // 根据节点类型分发
    if (auto component = std::dynamic_pointer_cast<ComponentNode>(node)) {
        visitComponent(*component);
    }
    // Property nodes are now handled inside visitComponent
}

void Generator::visitComponent(const ComponentNode& component) {
    // 检查是否是自定义组件的实例
    if (componentRegistry.count(component.type)) {
        // 这是一个自定义组件的实例
        auto original = componentRegistry.at(component.type);
        auto instance = deepCopyComponent(original);

        // 覆写属性和追加子节点
        for (const auto& child : component.children) {
            if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
                // 查找并覆写属性
                bool overridden = false;
                for (auto& existingChild : instance->children) {
                    if (auto existingProp = std::dynamic_pointer_cast<PropertyNode>(existingChild)) {
                        if (existingProp->name == prop->name) {
                            existingProp->value = prop->value;
                            overridden = true;
                            break;
                        }
                    }
                }
                if (!overridden) { // 如果不存在，则添加新属性
                    instance->children.push_back(child);
                }
            } else if (auto comp = std::dynamic_pointer_cast<ComponentNode>(child)) {
                // 追加子组件
                instance->children.push_back(child);
            }
        }

        // 用合并后的实例继续处理
        visitComponent(*instance);
        return;
    }


    // --- 以下是处理原生组件或已合并的自定义组件实例的逻辑 ---
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
    } else if (component.type == "Rect" || component.type == "Text" || component.type == "Item") {
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

    // 7. 闭合 HTML 标签 (原生组件)
    if (componentRegistry.find(component.type) == componentRegistry.end()) { // It's a primitive
        if (component.type == "Row" || component.type == "Col" || component.type == "Rect" || component.type == "Item") {
            htmlOutput << "</div>\n";
        } else if (component.type == "Text") {
            htmlOutput << "</span>\n";
        }
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
    else                                tag = "<div class=\"" + currentClassName + "\">"; // 自定义组件和其他默认为 div
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

void Generator::preprocessDefinitions(const ProgramNode& program) {
    for (const auto& node : program.children) {
        if (auto def = std::dynamic_pointer_cast<ComponentDefinitionNode>(node)) {
            componentRegistry[def->name] = def->body;
        }
    }
}

std::shared_ptr<ComponentNode> Generator::deepCopyComponent(const std::shared_ptr<ComponentNode>& original) {
    if (!original) return nullptr;

    auto copy = std::make_shared<ComponentNode>();
    copy->type = original->type;

    for (const auto& child : original->children) {
        if (auto prop = std::dynamic_pointer_cast<PropertyNode>(child)) {
            auto propCopy = std::make_shared<PropertyNode>();
            propCopy->name = prop->name;
            propCopy->value = prop->value;
            copy->children.push_back(propCopy);
        } else if (auto comp = std::dynamic_pointer_cast<ComponentNode>(child)) {
            copy->children.push_back(deepCopyComponent(comp));
        }
    }
    return copy;
}
