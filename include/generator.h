#pragma once
#include "ast.h"
#include <string>
#include <sstream>

namespace nota {

class Generator {
public:
    std::string generate(const std::shared_ptr<Component>& root);

private:
    std::stringstream html;
    std::stringstream css;
    int counter = 0;

    void generateComponent(const std::shared_ptr<Component>& comp);
    std::string generateStyles(const std::shared_ptr<Component>& comp, const std::string& className);
    std::string getValue(const std::shared_ptr<Expression>& expr);
};

}
