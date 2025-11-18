#pragma once

#include <string>

enum class ObjectType {
    STRING,
};

class Object {
public:
    explicit Object(ObjectType type);
    virtual ~Object() = default;

    ObjectType type;
    Object* next = nullptr;
};

class ObjectString : public Object {
public:
    explicit ObjectString(std::string&& chars);

    std::string chars;
};
