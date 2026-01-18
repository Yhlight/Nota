#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>

namespace nota {

struct Node {
    virtual ~Node() = default;
};

struct Expression : Node {
};

struct Literal : Expression {
    std::variant<int, double, std::string, bool> value;
    Literal(int v) : value(v) {}
    Literal(double v) : value(v) {}
    Literal(std::string v) : value(v) {}
    Literal(bool v) : value(v) {}
    Literal(const char* v) : value(std::string(v)) {}
};

struct Identifier : Expression {
    std::string name;
    Identifier(std::string n) : name(n) {}
};

struct ThisExpression : Expression {};
struct ParentExpression : Expression {};

struct BinaryExpression : Expression {
    std::shared_ptr<Expression> left;
    std::string op;
    std::shared_ptr<Expression> right;
    BinaryExpression(std::shared_ptr<Expression> l, std::string o, std::shared_ptr<Expression> r)
        : left(l), op(o), right(r) {}
};

struct MemberExpression : Expression {
    std::shared_ptr<Expression> object;
    std::string member;
    MemberExpression(std::shared_ptr<Expression> obj, std::string m)
        : object(obj), member(m) {}
};

struct GroupExpression : Expression {
    std::shared_ptr<Expression> expression;
    GroupExpression(std::shared_ptr<Expression> e) : expression(e) {}
};

struct Property : Node {
    std::string name;
    std::shared_ptr<Expression> value;
    Property(std::string n, std::shared_ptr<Expression> v) : name(n), value(v) {}
};

struct Component : Node {
    std::string type;
    std::string name; // For definitions like "Item Box" -> type="Item", name="Box"
    std::vector<std::shared_ptr<Property>> properties;
    std::vector<std::shared_ptr<Component>> children;

    Component(std::string t, std::string n = "") : type(t), name(n) {}

    void addProperty(std::shared_ptr<Property> p) {
        properties.push_back(p);
    }

    void addChild(std::shared_ptr<Component> c) {
        children.push_back(c);
    }
};

struct Program : Node {
    std::vector<std::shared_ptr<Component>> components; // List of top-level definitions/components

    void addComponent(std::shared_ptr<Component> c) {
        components.push_back(c);
    }
};

}
