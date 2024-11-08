#pragma once
#include "Node.h"

class Cube : public Node {
public:
    Cube() {}
    void accept(Visitor& visitor) override;

private:

};