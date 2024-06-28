#pragma once

#include <raylib.h>
#include <raymath.h>

#include <string>
#include <optional>

namespace bs
{

class NodeCircle
{
public:
    NodeCircle(int key, std::size_t complete_index, bool red);

public:
    void render() const;

private:
    float font_size() const;
    float font_spacing() const;

private:
    std::string _key;
    std::size_t _complete_index;

    Vector2 _position;
    std::optional<Vector2> _parent_position;
    Color _color;
};

} // namespace bs
