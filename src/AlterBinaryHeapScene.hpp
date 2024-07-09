#pragma once

#include "Scene.hpp"

#include <vector>

#include "NodeCircle.hpp"

#include "AlterBinaryHeap.hpp"
#include "InputBox.hpp"

namespace bs
{

class AlterBinaryHeapScene final : public Scene
{
public:
    AlterBinaryHeapScene();

public:
    auto update() -> std::optional<SceneType> override;
    void render() const override;

private:
    void redraw_tree();

    void on_number_input(int number);

private:
    struct MyData
    {
        int priority;
        int id;

        int unique_id() const
        {
            return id;
        }
        bool operator<(const MyData& other) const
        {
            return priority < other.priority;
        }
    };

    int _next_id = -1;
    int _selected_id = 0;

    AlterBinaryHeap<MyData> _heap;

    bool _valid;

    std::string _valid_str;

    InputBox _input_box;
    std::vector<NodeCircle> _node_circles;
};

} // namespace bs
