#ifndef NOTA_ARRAY_H
#define NOTA_ARRAY_H

#include "Value.h"
#include <vector>

namespace nota {

class NotaArray {
  public:
    NotaArray(std::vector<Value> elements);

    Value get(long long index);
    void set(long long index, Value value);
    long long size();

  private:
    std::vector<Value> elements;
};

} // namespace nota

#endif // NOTA_ARRAY_H
