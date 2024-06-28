#pragma once

#include <vector>
#include <random>

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
    void redraw_tree();

private:
    BSTree<int, int> _tree;

    std::mt19937 _rand;

    std::vector<NodeCircle> _node_circles;
};

} // namespace bs
