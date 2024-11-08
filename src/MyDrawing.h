#pragma once
#include "Visitor.h"
#include <glm/glm.hpp>

class MyDrawing : public Visitor {
public:
    MyDrawing(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
        : m_viewMatrix(viewMatrix), m_projectionMatrix(projectionMatrix) {}

    void visit(Transform& transform) override;
    void visit(Cube& cube) override;
    void visit(Camera& camera) override;

private:
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_currentTransform = glm::mat4(1.0f);
};