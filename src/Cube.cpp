#include "Cube.h"
#include "Visitor.h"

void Cube::accept(Visitor& visitor) {
    visitor.visit(*this);
}