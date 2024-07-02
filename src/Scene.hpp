#pragma once

#include <vector>

#include "NodeCircle.hpp"

#include "InputBox.hpp"
#include "RBTree.hpp"

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
    RBTree<int, int> _tree;

    int _black_depth;
    bool _valid;

    std::string _black_depth_str;
    std::string _valid_str;

    InputBox _input_box;
    std::vector<NodeCircle> _node_circles;
};

} // namespace bs
