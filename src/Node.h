#pragma once
#include <vector>
#include <memory>
#include "Visitor.h"

class Node {
public:
    Node* getParent() const { return m_parent; }
    void accept(Visitor& visitor);

protected:
    Node* m_parent;
};