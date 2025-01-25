#include "raylib.h"

int player1score = 0 ;
int player2score = 0 ;

Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

bool menu() ;

class Ball 
{
public:
    float x,y ;
    int speed_x,speed_y ;
    int radius ;

    void Draw() 
    {
        DrawCircle(x,y,radius,Yellow);
    }

    bool update()
    {
        x += speed_x ;
        y += speed_y ;

        if ( x+radius >= GetScreenWidth() )
        {
            player1score ++ ;
            if (player1score == 3) return false ;
            ResetBall() ;

        }
        if (x-radius <= 0)
        {
            player2score ++ ;
            if (player2score == 3) return false ;
            ResetBall();
        }

        if ( y+radius >= GetScreenHeight() || y-radius <= 0)
        {
            speed_y *= -1 ;
        }

        return true ;
    }

    void ResetBall()
    {
        x = GetScreenWidth()/2 ;
        y = GetScreenHeight()/2 ;

        int speed_choices[2] = {-1,1} ;
        speed_x *= speed_choices[GetRandomValue(0,1)];
        speed_y *= speed_choices[GetRandomValue(0,1)];
    }
};

class Paddle
{
public:
    float x, y ;
    float width , height ;
    int speed ;

    void Draw()
    {
        DrawRectangleRounded(Rectangle{x,y,width,height},0.8,0,WHITE);
    }

    void update_for_player1()
    {
        if (IsKeyDown(KEY_DOWN))
        {
            y = y + speed ;
        }
        if (IsKeyDown(KEY_UP))
        {
            y = y - speed ;
        }

        if (y <= 0 )
        {
            y = 0 ;
        }
        else if ( y + height >= GetScreenHeight())
        {
            y = GetScreenHeight() - height ;
        }
    }

    void update_for_player2()
    {
        if (IsKeyDown(KEY_S))
        {
            y = y + speed ;
        }
        if (IsKeyDown(KEY_W))
        {
            y = y - speed ;
        }

        if (y <= 0 )
        {
            y = 0 ;
        }
        else if ( y + height >= GetScreenHeight())
        {
            y = GetScreenHeight() - height ;
        }
    }
};

int run()
{
    Ball ball ;
    Paddle player1 ;
    Paddle player2 ;


    int screen_width , screen_height ;
    screen_width = 1280 ;
    screen_height = 800 ;

    InitWindow(screen_width,screen_height,"Amazing's Pong Game");
    SetTargetFPS(60) ;

    ball.radius = 20 ;
    ball.x = screen_width/2 ;
    ball.y = screen_height/2 ;
    ball.speed_x = 7 ;
    ball.speed_y = 7 ;

    player1.height = 120 ;
    player1.width = 25 ;
    player1.x = screen_width - player1.width - 10 ; // 1280 - 25 - 10  = 
    player1.y = screen_height/2 - player1.height/2 ;
    player1.speed = 6 ;

    player2.height = 120 ;
    player2.width = 25 ;
    player2.x = player1.width  ;
    player2.y = screen_height/2 - player2.height/2 ;
    player2.speed = 6 ;


    while(!WindowShouldClose()) 
    {
        if (IsKeyDown(KEY_ESCAPE))
        {
            break;
        }

        bool winner = ball.update()  ;
        if (winner == false)
        {
            ClearBackground(BLACK) ;
            
            break ;
        }
        player1.update_for_player1() ;
        player2.update_for_player2() ;
        ClearBackground(Dark_Green) ;

        BeginDrawing() ;

        if (CheckCollisionCircleRec(Vector2{ball.x,ball.y},ball.radius,Rectangle{player1.x,player1.y,player1.width,player1.height}))
        {
            ball.speed_x *= -1 ;
        }
        if (CheckCollisionCircleRec(Vector2{ball.x,ball.y},ball.radius,Rectangle{player2.x,player2.y,player2.width,player2.height}))
        {
            ball.speed_x *= -1 ;
        }
        

        DrawRectangle(screen_width/2 , 0 ,screen_width/2 , screen_height , Green) ;
        DrawCircle(screen_width/2 , screen_height/2 , 150 , Light_Green);

        DrawLine(screen_width/2 , 0 , screen_width/2 , screen_height,WHITE);
        ball.Draw() ;
        player1.Draw() ;
        player2.Draw() ;
        DrawText(TextFormat("%i",player1score),screen_width/4 -20,20,80,WHITE);
        DrawText(TextFormat("%i",player2score),3*screen_width/4 -20,20,80,WHITE);


        EndDrawing() ;
    }

    CloseWindow() ;
    return 0 ;
}