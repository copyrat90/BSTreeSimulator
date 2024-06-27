#include <raylib.h>

#include <cmath>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

void update_draw_frame(void); // Update and Draw one frame

namespace
{
constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
} // namespace

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Binary Search Tree Simulator");

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(update_draw_frame, 0, 1);
#else
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        update_draw_frame();
    }
#endif

    CloseWindow(); // Close window and OpenGL context

    return 0;
}

void update_draw_frame(void)
{
    // update
    struct Pos
    {
        float x, y;
    };

    static constexpr float SPEED = 2.0f;
    static constexpr float RADIUS = 10.0f;

    static Pos player = {.x = RADIUS, .y = RADIUS};

    if (IsKeyDown(KEY_LEFT))
        player.x -= SPEED;
    if (IsKeyDown(KEY_RIGHT))
        player.x += SPEED;
    if (IsKeyDown(KEY_UP))
        player.y -= SPEED;
    if (IsKeyDown(KEY_DOWN))
        player.y += SPEED;

    // draw
    BeginDrawing();
    {
        ClearBackground(LIGHTGRAY);

        // draw static circles
        DrawCircle(100, 60, RADIUS, BLACK);
        DrawCircle(60, 80, RADIUS, RED);
        DrawCircle(140, 80, RADIUS, RED);
        DrawCircle(40, 100, RADIUS, BLACK);
        DrawCircle(80, 100, RADIUS, BLACK);
        DrawCircle(120, 100, RADIUS, BLACK);
        DrawCircle(160, 100, RADIUS, BLACK);

        // draw player
        DrawCircle(std::round(player.x), std::round(player.y), RADIUS, BLUE);

        DrawText("Congrats! You created your first window!", 190, 200, 20, RAYWHITE);

        DrawFPS(10, 10);
    }
    EndDrawing();
}
