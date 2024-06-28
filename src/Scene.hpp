#pragma once

#include <vector>

#include "NodeCircle.hpp"

#include "BSTree.hpp"

namespace bs
{

class Scene final
{
public:
    Scene();

public:
    void update();
    void render() const;

private:
    BSTree<int, int> _tree;

    std::vector<NodeCircle> _node_circles;
};

} // namespace bs
