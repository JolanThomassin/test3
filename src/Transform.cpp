#include "Transform.h"
#include "Visitor.h"

void Transform::accept(Visitor& visitor) {
    visitor.visit(*this);
}