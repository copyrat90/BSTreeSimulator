#pragma once

#include <raylib.h>

#include <functional>
#include <string>

namespace bs
{

class InputBox final
{
public:
    InputBox(Vector2 position, std::function<void(int)> input_callback);

public:
    void update();
    void render() const;

private:
    Vector2 _position;
    std::function<void(int)> _input_callback;

    std::string _str;
};

} // namespace bs
