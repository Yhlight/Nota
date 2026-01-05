#pragma once
#include <string>

class Nota {
public:
    Nota();
    void run(const std::string& source, std::string& html_output, std::string& css_output);
};
