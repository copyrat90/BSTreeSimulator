#pragma once

#include <raylib.h>
#include <raymath.h>

#include <optional>
#include <string>

namespace bs
{

class NodeCircle
{
public:
    NodeCircle(int key, std::size_t complete_index, bool red);
    NodeCircle(int key, int id, std::size_t complete_index, bool red);

public:
    void render() const;

public:
    bool is_red() const;
    void set_red(bool red);

private:
    float font_size() const;
    float font_spacing() const;

private:
    std::string _key;
    std::string _id;
    std::size_t _complete_index;

    Vector2 _position;
    std::optional<Vector2> _parent_position;
    bool _red;
};

} // namespace bs
