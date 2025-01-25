#include "grid.h"
#include "raylib.h"



int main()
{
    Color darkblue = {44, 44, 127, 255};

    InitWindow(300, 600, "Tetris");
    SetTargetFPS(60);

    Grid grid = Grid();
    grid.grid[0][0] = 1 ;
    grid.grid[3][5] = 4 ;
    grid.grid[17][8] = 7 ;
    grid.Print();

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(darkblue);
        grid.Draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
