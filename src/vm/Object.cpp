#include "Object.h"

Object::Object(ObjectType type) : type(type) {}

ObjectString::ObjectString(std::string&& chars)
    : Object(ObjectType::STRING), chars(std::move(chars)) {}