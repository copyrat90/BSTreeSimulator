#pragma once

#include <vector>

#include "NodeCircle.hpp"

#include "BSTree.hpp"
#include "InputBox.hpp"

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

    void on_number_input(int number);

private:
    BSTree<int, int> _tree;

    InputBox _input_box;
    std::vector<NodeCircle> _node_circles;
};

} // namespace bs
