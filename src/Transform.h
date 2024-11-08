#pragma once
#include "Node.h"
#include <glm/glm.hpp>
#include <vector>

class Transform : public Node {
public:
    Transform(const glm::mat4& matrix = glm::mat4(1.0f), Node* parent = nullptr)
        : Node(parent), m_matrix(matrix) {}

    void addChild(std::unique_ptr<Node> child) {
        child->m_parent = this;
        m_children.push_back(child);
    }

    const glm::mat4& getMatrix() const { return m_matrix; }
    const std::vector<std::unique_ptr<Node>>& getChildren() const { return m_children; }

    void accept(Visitor& visitor) override;

private:
    glm::mat4 m_matrix;
    std::vector<std::unique_ptr<Node>> m_children;
};