import sys

def define_visitor(f, base_name, types):
    f.write(f"template <typename R>\n")
    f.write(f"class {base_name}Visitor {{\n")
    f.write("public:\n")
    for type_name in types:
        f.write(f"    virtual R visit{type_name}{base_name}(const {type_name}& {base_name.lower()}) = 0;\n")
    f.write(f"    virtual ~{base_name}Visitor() = default;\n")
    f.write("};\n\n")

def define_type(f, base_name, class_name, fields):
    f.write(f"class {class_name} : public {base_name} {{\n")
    f.write("public:\n")
    # Constructor
    f.write(f"    {class_name}(")
    field_list = [f"{field.split(' ')[0]} {field.split(' ')[1]}" for field in fields.split(", ")]
    f.write(", ".join(field_list))
    f.write(") :\n")
    # Initializer list
    initializer_list = [f"        {field.split(' ')[1]}(std::move({field.split(' ')[1]}))" for field in fields.split(", ")]
    f.write(",\n".join(initializer_list))
    f.write(" {}\n\n")

    # Visitor accept method
    f.write("    std::any accept(ExprVisitor<std::any>& visitor) const override {\n")
    f.write(f"        return visitor.visit{class_name}{base_name}(*this);\n")
    f.write("    }\n\n")


    # Fields
    for field in fields.split(", "):
        f.write(f"    const {field};\n")
    f.write("};\n\n")

def define_ast(output_dir, base_name, types):
    path = f"{output_dir}/{base_name}.h"
    with open(path, "w") as f:
        f.write("#ifndef EXPR_H\n")
        f.write("#define EXPR_H\n\n")
        f.write("#include \"Token.h\"\n")
        f.write("#include <memory>\n")
        f.write("#include <vector>\n")
        f.write("#include <any>\n\n")

        # Forward declarations
        for type_name in types:
            f.write(f"class {type_name};\n")
        f.write("\n")

        define_visitor(f, base_name, types)

        f.write(f"class {base_name} {{\n")
        f.write("public:\n")
        f.write(f"    virtual ~{base_name}() = default;\n")
        f.write(f"    virtual std::any accept({base_name}Visitor<std::any>& visitor) const = 0;\n")
        f.write("};\n\n")

        for type_name, fields in types.items():
            define_type(f, base_name, type_name, fields)

        f.write("#endif // EXPR_H\n")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: generate_ast <output_directory>")
        sys.exit(64)
    output_dir = sys.argv[1]
    define_ast(output_dir, "Expr", {
        "Binary": "std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right",
        "Grouping": "std::unique_ptr<Expr> expression",
        "Literal": "std::any value",
        "Unary": "Token op, std::unique_ptr<Expr> right"
    })
