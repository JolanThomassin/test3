#include "MyDrawing.h"
#include "Transform.h"
#include "Cube.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

void MyDrawing::visit(Transform& transform) {
    // Sauvegarder la transformation actuelle
    glm::mat4 parentTransform = m_currentTransform;

    // Multiplier la transformation actuelle par celle du nœud
    m_currentTransform *= transform.getMatrix();

    // Parcourir les enfants
    for (Node& child : transform.getChildren()) {
        child->accept(*this);
    }

    m_currentTransform = parentTransform;
}

void MyDrawing::visit(Cube& cube) {
    glm::mat4 modelView = m_viewMatrix * m_currentTransform;
    glm::mat4 projection = m_projectionMatrix;
}

void MyDrawing::visit(Camera& camera) {
    
}