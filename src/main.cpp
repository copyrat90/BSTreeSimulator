#include <raylib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include "Scene.hpp"
#include "constants.hpp"

bs::Scene g_scene;

void update_draw_frame(void); // Update and Draw one frame

int main()
{
    InitWindow(bs::WINDOW_WIDTH, bs::WINDOW_HEIGHT, "Binary Search Tree Simulator");

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
    g_scene.update();

    BeginDrawing();
    {
        ClearBackground(LIGHTGRAY);
        g_scene.render();
        DrawFPS(10, 10);
    }
    EndDrawing();
}
