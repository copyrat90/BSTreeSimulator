#include "Scene.hpp"

#include <format>

namespace bs
{

namespace
{
constexpr const char* BLACK_DEPTH_FMT = "black depth: {}";
constexpr const char* VALID_FMT = "valid: {}";
} // namespace

Scene::Scene()
    : _black_depth(_tree.black_depth()), _valid(_tree.validate()),
      _black_depth_str(std::format(BLACK_DEPTH_FMT, _black_depth)), _valid_str(std::format(VALID_FMT, _valid)),
      _input_box({80, 50}, [this](int num) { on_number_input(num); })
{
}

void Scene::update()
{
    _input_box.update();
}

void Scene::render() const
{
    for (const auto& circle : _node_circles)
        circle.render();

    _input_box.render();

    DrawText(_black_depth_str.c_str(), 80, 110, 30, (_black_depth >= 0 ? BLACK : RED));
    DrawText(_valid_str.c_str(), 80, 150, 30, (_valid ? BLACK : RED));
}

void Scene::redraw_tree()
{
    _node_circles.clear();

    _tree.postorder([this](int key, [[maybe_unused]] int val, const bs::TraversalInfo& info) {
        _node_circles.emplace_back(key, info.complete_index, info.red);
    });

    _black_depth = _tree.black_depth();
    _valid = _tree.validate();

    _black_depth_str = std::format(BLACK_DEPTH_FMT, _black_depth);
    _valid_str = std::format(VALID_FMT, _valid);
}

void Scene::on_number_input(int number)
{
    // erase if `number` already exists
    if (!_tree.insert(number, number))
        _tree.erase(number);

    redraw_tree();
}

} // namespace bs
