#ifndef NOTA_OBJECT_H
#define NOTA_OBJECT_H

#include "Chunk.h"
#include <string>

namespace nota {

class NotaFunction {
public:
    NotaFunction(std::string name, int arity);

    std::string name;
    int arity;
    Chunk chunk;
};

} // namespace nota

#endif // NOTA_OBJECT_H
