#include "NodeCircle.hpp"

#include <bit>
#include <cmath>
#include <utility>

namespace bs
{

namespace
{

constexpr float FONT_RADIUS_RATIO = 1.0f;
constexpr float LINE_THICKNESS = 2.0f;

constexpr float RADIUS = 20;
constexpr int MAX_LEVEL_TO_DRAW = 12;

auto complete_index_to_position(std::size_t complete_index) -> Vector2
{
    const auto level = std::bit_width(complete_index + 1) - 1;
    const auto column = (complete_index + 1) - (1ULL << level);

    const auto x = GetScreenWidth() * (2 * column + 1) / (float)(1ULL << (level + 1));
    const auto y = GetScreenHeight() * (level + 1) / (float)(MAX_LEVEL_TO_DRAW + 1);

    return {x, y};
}

} // namespace

NodeCircle::NodeCircle(int key, std::size_t complete_index, bool red)
    : _key(std::to_string(key)), _complete_index(complete_index), _position(complete_index_to_position(complete_index)),
      _color(red ? RED : BLACK)
{
    // only calculate parent position when not root
    if (complete_index != 0)
        _parent_position = complete_index_to_position((complete_index + 1) / 2 - 1);
}

void NodeCircle::render() const
{
    if (_parent_position)
        DrawLineEx(*_parent_position, _position, LINE_THICKNESS, BLACK);
    DrawCircle((int)std::round(_position.x), (int)std::round(_position.y), RADIUS, _color);

    const auto text_scale = MeasureTextEx(GetFontDefault(), _key.c_str(), font_size(), font_spacing());
    const auto center_pos = Vector2Add(_position, Vector2Scale(text_scale, -0.5f));

    DrawTextEx(GetFontDefault(), _key.c_str(), center_pos, font_size(), font_spacing(), WHITE);
}

float NodeCircle::font_size() const
{
    return RADIUS * FONT_RADIUS_RATIO;
}

float NodeCircle::font_spacing() const
{
    return font_size() / 10;
}

} // namespace bs
