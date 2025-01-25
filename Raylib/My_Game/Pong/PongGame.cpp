#include "raylib.h"
#include "game.h"
#include <cstring>

bool menu()
{
    InitWindow(1280,800,"Welcome To The Pong Game") ;


    const char *message = "Welcome To The Amazing's Pong Game!";
    int charIndex = 0;         // 当前显示字符的索引
    float delay = 0.04f;        // 每个字符的延迟时间（秒）
    float timer = 0.0f;        // 计时器

    SetTargetFPS(60); // 设置帧率
    
    Rectangle playButton = { 1280/2 -100, 800/2 , 200, 50 };
    Rectangle quitButton = { 1280/2 -100, 870/2 +20 , 200, 50 };

    
    
    while(!WindowShouldClose())
    {

        timer += GetFrameTime();
        if (timer >= delay && charIndex < (int)strlen(message)) {
            charIndex++;
            timer = 0.0f; // 重置计时器
        }


        BeginDrawing();
        ClearBackground(BLACK) ;
        DrawText(TextSubtext(message, 0, charIndex), 1280/2 - 380, 800/2 - 200 , 40, Yellow);

        DrawRectangleRec(playButton, Yellow);
        DrawText("Play", playButton.x + 70, playButton.y + 15, 30, WHITE);

        DrawRectangleRec(quitButton, Yellow);
        DrawText("Quit", quitButton.x + 70, quitButton.y + 15, 30, WHITE);

        DrawText("Press ESC to Exit", 10, 10, 20, GRAY);

        if (IsKeyDown(KEY_ESCAPE))
        {
            CloseWindow() ;
            return false ;

        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // 如果点击了 Play 按钮
            if (CheckCollisionPointRec(GetMousePosition(), playButton)) {
                // 执行 Play 按钮的操作
                return true ;
            }
            // 如果点击了 Quit 按钮
            if (CheckCollisionPointRec(GetMousePosition(), quitButton)) {
                // 执行 Quit 按钮的操作
                CloseWindow(); // 退出游戏
                return false ;
            }
        } 

        EndDrawing() ;
    }
    CloseWindow();
    return false ;

}



int main()
{
    bool run_the_game = menu() ;
    if (run_the_game)  run() ;
}