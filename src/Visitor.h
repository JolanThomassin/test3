#pragma once
#include "Transform.h"
#include "Cube.h"
#include "Camera.h"

class Visitor {
public:
    void visit(Transform& transform);
    void visit(Cube& cube);
    void visit(Camera& camera);
};