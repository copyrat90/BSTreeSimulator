#include <raylib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include <format>
#include <memory>
#include <stdexcept>

#include "AlterBinaryHeapScene.hpp"
#include "BSTreeScene.hpp"
#include "SceneType.hpp"

std::unique_ptr<bs::Scene> g_scene;

void update_draw_frame(void); // Update and Draw one frame

int main()
{
    InitWindow(1792, 1008, "Binary Search Tree Simulator");

    g_scene.reset(new bs::BSTreeScene);

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
    const auto next_scene = g_scene->update();

    BeginDrawing();
    {
        ClearBackground(LIGHTGRAY);
        g_scene->render();
        DrawFPS(10, 10);
    }
    EndDrawing();

    if (next_scene.has_value())
    {
        switch (next_scene.value())
        {
        case bs::SceneType::BSTREE:
            g_scene.reset();
            g_scene.reset(new bs::BSTreeScene);
            break;
        case bs::SceneType::ALTER_BINARY_HEAP:
            g_scene.reset();
            g_scene.reset(new bs::AlterBinaryHeapScene);
            break;

        default:
            throw std::logic_error(std::format("Invalid scene type={}", (int)next_scene.value()));
        }
    }
}
