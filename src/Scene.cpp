#include "Scene.hpp"

namespace bs
{

Scene::Scene() : _rand(std::random_device{}())
{
}

void Scene::update()
{
    if (IsKeyPressed(KEY_Z))
    {
        std::uniform_int_distribution<int> dist(0, 10000);
        const int key = dist(_rand);
        _tree.insert(key, key);
        redraw_tree();
    }
}

void Scene::render() const
{
    for (const auto& circle : _node_circles)
        circle.render();
}

void Scene::redraw_tree()
{
    _node_circles.clear();

    _tree.postorder([this](int key, [[maybe_unused]] int val, std::size_t complete_index) {
        _node_circles.emplace_back(key, complete_index, false);
    });
}

} // namespace bs
