#pragma once

#include "NotaObject.hpp"
#include <string>

namespace nota {
namespace core {

class StringObject : public NotaObject {
public:
    explicit StringObject(const std::string& value) : NotaObject(ObjectType::STRING), value_(value) {}

    const std::string& GetValue() const { return value_; }

private:
    std::string value_;
};

} // namespace core
} // namespace nota
