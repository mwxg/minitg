#include <raylib.h>

int main()
{
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "MiniTG");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);

        DrawText("raylib testing", 280, 200, 30, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

using namespace std;
