#ifndef NOTA_VALUE_HPP
#define NOTA_VALUE_HPP

#include <variant>
#include <string>

namespace nota {

    using Value = std::variant<std::monostate, bool, double, std::string>;

} // namespace nota

#endif // NOTA_VALUE_HPP
