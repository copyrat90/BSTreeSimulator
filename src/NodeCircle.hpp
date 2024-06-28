#pragma once

#include <raylib.h>
#include <raymath.h>

#include <string>

namespace bs
{

struct NodeCircle
{
public:
    NodeCircle(std::string&& key, Vector2 position, float radius, Color color);

public:
    void render() const;

private:
    float font_size() const;
    float font_spacing() const;

public:
    std::string key;

    Vector2 position; // center position
    float radius;
    Color color;
};

} // namespace bs
