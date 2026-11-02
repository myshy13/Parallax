#include "raylib.h"

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Parallax");
#ifndef __EMSCRIPTEN__
    // Relative paths resolve against the copied shaders/ and assets/, wherever this was launched from.
    ChangeDirectory(GetApplicationDirectory());
#endif
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);
        DrawText("hello", 20, 20, 40, BLACK);
        EndDrawing();
    }

    CloseWindow();
}
