#include "Scene.hpp"

namespace bs
{

Scene::Scene() : _input_box({100, 50}, [this](int num) { on_number_input(num); })
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
}

void Scene::redraw_tree()
{
    _node_circles.clear();

    _tree.postorder([this](int key, [[maybe_unused]] int val, std::size_t complete_index) {
        _node_circles.emplace_back(key, complete_index, true);
    });
}

void Scene::on_number_input(int number)
{
    // erase if `number` already exists
    if (!_tree.insert(number, number))
        _tree.erase(number);

    redraw_tree();
}

} // namespace bs
