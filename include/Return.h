#ifndef NOTA_RETURN_H
#define NOTA_RETURN_H

#include "Value.h"
#include <stdexcept>

namespace nota {

class Return : public std::runtime_error {
  public:
    Return(Value value);

    Value value;
};

} // namespace nota

#endif // NOTA_RETURN_H
