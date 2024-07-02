#include "Scene.hpp"

#include <format>

namespace bs
{

namespace
{
constexpr const char* BLACK_DEPTH_FMT = "black depth: {}";
}

Scene::Scene()
    : _black_depth_str(std::format(BLACK_DEPTH_FMT, _tree.black_depth())),
      _input_box({100, 50}, [this](int num) { on_number_input(num); })
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
    DrawText(_black_depth_str.c_str(), 100, 110, 30, BLACK);
}

void Scene::redraw_tree()
{
    _node_circles.clear();

    _tree.postorder([this](int key, [[maybe_unused]] int val, const bs::TraversalInfo& info) {
        _node_circles.emplace_back(key, info.complete_index, info.red);
    });

    _black_depth_str = std::format(BLACK_DEPTH_FMT, _tree.black_depth());
}

void Scene::on_number_input(int number)
{
    // erase if `number` already exists
    if (!_tree.insert(number, number))
        _tree.erase(number);

    redraw_tree();
}

} // namespace bs
