#include "NotaArray.h"
#include <stdexcept>

namespace nota {

NotaArray::NotaArray(std::vector<Value> elements) : elements(elements) {}

Value NotaArray::get(long long index) {
    if (index < 0 || index >= elements.size()) {
        throw std::runtime_error("Array index out of bounds.");
    }
    return elements[index];
}

void NotaArray::set(long long index, Value value) {
    if (index < 0 || index >= elements.size()) {
        throw std::runtime_error("Array index out of bounds.");
    }
    elements[index] = value;
}

} // namespace nota
