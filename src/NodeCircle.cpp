#include "NodeCircle.hpp"

#include <cmath>
#include <utility>

namespace bs
{

namespace
{
constexpr float FONT_RADIUS_RATIO = 0.4f;
} // namespace

NodeCircle::NodeCircle(std::string&& key, Vector2 position, float radius, Color color)
    : key(std::move(key)), position(position), radius(radius), color(color)
{
}

void NodeCircle::render() const
{
    DrawCircle((int)std::round(position.x), (int)std::round(position.y), radius, color);

    const auto text_scale = MeasureTextEx(GetFontDefault(), key.c_str(), font_size(), font_spacing());
    const auto center_pos = Vector2Add(position, Vector2Scale(text_scale, -0.5f));

    DrawTextEx(GetFontDefault(), key.c_str(), center_pos, font_size(), font_spacing(), WHITE);
}

float NodeCircle::font_size() const
{
    return radius * FONT_RADIUS_RATIO;
}

float NodeCircle::font_spacing() const
{
    return font_size() / 10;
}

} // namespace bs
