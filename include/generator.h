#pragma once
#include "ast.h"
#include <string>
#include <sstream>

#include <map>

namespace nota {

class Generator {
public:
    std::string generate(const std::shared_ptr<Program>& program);
    // Overload for single component testing
    std::string generate(const std::shared_ptr<Component>& root);

private:
    std::stringstream html;
    std::stringstream css;
    int counter = 0;
    std::map<std::string, std::shared_ptr<Component>> definitions;

    void generateComponent(const std::shared_ptr<Component>& comp);
    std::string generateStyles(const std::shared_ptr<Component>& comp, const std::string& className);
    std::string getValue(const std::shared_ptr<Expression>& expr, bool addPx = true);

    std::shared_ptr<Component> expandComponent(const std::shared_ptr<Component>& instance);
};

}
