#include "AlterBinaryHeapScene.hpp"

#include <raylib.h>

#include <format>

#include "SceneType.hpp"

namespace bs
{

namespace
{
constexpr const char* VALID_FMT = "valid: {}";
}

AlterBinaryHeapScene::AlterBinaryHeapScene()
    : Scene(SceneType::ALTER_BINARY_HEAP), _valid(_heap.validate()), _valid_str(std::format(VALID_FMT, _valid)),
      _input_box({80, 50}, [this](int num) { on_number_input(num); })
{
}

auto AlterBinaryHeapScene::update() -> std::optional<SceneType>
{
    _input_box.update();

    if (IsKeyPressed(KEY_DELETE) && !_heap.empty())
    {
        _heap.pop();
        redraw_tree();
    }

    if (IsKeyPressed(KEY_SPACE))
        return SceneType::BSTREE;

    return std::nullopt;
}

void AlterBinaryHeapScene::render() const
{
    for (const auto& circle : _node_circles)
        circle.render();

    _input_box.render();

    DrawText(_valid_str.c_str(), 80, 110, 30, (_valid ? BLACK : RED));

    DrawText("Binary Heap", 400, 50, 30, BLACK);

    DrawText("[Delete] remove top", 1300, 30, 30, BLACK);
    DrawText("[Space] Go to BST", 1300, 70, 30, BLACK);
}

void AlterBinaryHeapScene::redraw_tree()
{
    _node_circles.clear();

    for (int heap_idx = _heap.size() - 1; heap_idx >= 0; --heap_idx)
    {
        const auto& data = _heap.begin()[heap_idx];
        _node_circles.emplace_back(data.priority, data.id, heap_idx, _selected_id == data.id);
    }

    _valid = _heap.validate();
    _valid_str = std::format(VALID_FMT, _valid);
}

void AlterBinaryHeapScene::on_number_input(int number)
{
    // select id
    if (number < 0)
    {
        // deselect previous selection
        if (_selected_id)
        {
            _selected_id = 0;
        }

        const auto it = _heap.find(number);

        // select new selection
        if (it != _heap.cend())
        {
            _selected_id = number;
        }
    }
    // change the selected id's priority
    else if (_selected_id)
    {
        // update priority of it
        _heap.push(MyData{.priority = number, .id = _selected_id});

        _selected_id = 0;
    }
    // insert positive number
    else
    {
        _heap.push(MyData{.priority = number, .id = _next_id--});
    }

    redraw_tree();
}

} // namespace bs
