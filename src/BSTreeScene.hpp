#pragma once

#include "Scene.hpp"

#include <vector>

#include "NodeCircle.hpp"

#include "InputBox.hpp"
#include "RBTree.hpp"

namespace bs
{

class BSTreeScene final : public Scene
{
public:
    BSTreeScene();

public:
    auto update() -> std::optional<SceneType> override;
    void render() const override;

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
