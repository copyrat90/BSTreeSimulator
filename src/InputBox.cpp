#include "InputBox.hpp"

#include <stdexcept>

namespace bs
{

namespace
{

}

InputBox::InputBox(Vector2 position, std::function<void(int)> input_callback)
    : _position(position), _input_callback(input_callback)
{
}

void InputBox::update()
{
    int unicode_char;
    while ((unicode_char = GetCharPressed()))
    {
        if (_str.size() >= 9)
            continue;
        else if (unicode_char == '-')
        {
            if (_str.empty())
                _str.push_back('-');
        }
        else if ('0' <= unicode_char && unicode_char <= '9')
            _str.push_back((char)unicode_char);
    }

    if (IsKeyPressed(KEY_ENTER) && !_str.empty())
    {
        try
        {
            const int num = std::stoi(_str);
            _input_callback(num);
        }
        catch (const std::invalid_argument&)
        {
            // do nothing
        }

        _str.clear();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !_str.empty())
    {
        _str.pop_back();
    }
}

void InputBox::render() const
{
    Rectangle bg_box = {.x = _position.x, .y = _position.y, .width = 225, .height = 50};

    DrawRectangleRec(bg_box, RAYWHITE);

    DrawTextEx(GetFontDefault(), (_str.empty() ? "Number..." : _str.c_str()), _position, 40, 4,
               (_str.empty() ? LIGHTGRAY : BLACK));

    DrawRectangleLinesEx(bg_box, 1, BLACK);
}

} // namespace bs
