#include "Scene.hpp"

namespace bs
{

Scene::Scene()
{
    // test
    static constexpr float RADIUS = 50;
    _node_circles.emplace_back("root", Vector2{500, 100}, RADIUS, BLACK);
    _node_circles.emplace_back("left_sub", Vector2{300, 200}, RADIUS, RED);
    _node_circles.emplace_back("right_sub", Vector2{700, 200}, RADIUS, RED);
    _node_circles.emplace_back("1", Vector2{200, 300}, RADIUS, BLACK);
    _node_circles.emplace_back("2", Vector2{400, 300}, RADIUS, BLACK);
    _node_circles.emplace_back("3", Vector2{600, 300}, RADIUS, BLACK);
    _node_circles.emplace_back("4", Vector2{800, 300}, RADIUS, BLACK);
}

void Scene::update()
{
}

void Scene::render() const
{
    for (const auto& circle : _node_circles)
        circle.render();
}

} // namespace bs
