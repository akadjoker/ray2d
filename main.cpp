#include <iostream>
using namespace std;
#include "Core.h"





#define MAX_BUNNIES        50000    // 50K bunnies limit

// This is the maximum amount of elements (quads) per batch
// NOTE: This value is defined in [rlgl] module and can be changed there
#define MAX_BATCH_ELEMENTS  8192
int bunniesCount = 0;           // Bunnies counter


class  Bunny  :public GameObject
{

public:

Vector2 speed;




  void OnCreate() override
 {
  TraceLog(LOG_WARNING, "***********on create bunny.");

  transform->position = GetMousePosition();
  speed.x = (float)GetRandomValue(-250, 250)/60.0f;
  speed.y = (float)GetRandomValue(-250, 250)/60.0f;
  SpriteComponent* spr =GetComponent<SpriteComponent>();
  spr->color=(Color){ GetRandomValue(50, 240),
                                                       GetRandomValue(80, 240),
                                                       GetRandomValue(100, 240), 255 };

 }

 void OnUpdate(float dt)override
 {


           transform->position.x += speed.x;
           transform->position.y += speed.y;

            if (((transform->position.x + width/2) > 1200) ||
                ((transform->position.x + width/2) < 0)) speed.x *= -1;
            if (((transform->position.y + height/2) > 750) ||
                ((transform->position.y + height/2 - 40) < 0)) speed.y *= -1;



 }

};

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1200;
    const int screenHeight = 750;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera");

    Rectangle player = {0, 0, 40, 40 };


    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.x + 20.0f, player.y + 20.0f };
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second



    Texture2D texture = LoadTexture("assets/wabbit_alpha.png");


    Group maingroup;





    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {

        for (int i = 0; i < 100; i++)
            {
                   GameObject* bunny=(GameObject*)new Bunny();// maingroup.AddGameObject("bunn");
                   bunny->AddComponent<SpriteComponent>(texture);
                   bunny->setCenterPivot();
                   maingroup.AddGameObject(bunny);
                   }

        }
        //----------------------------------------------------------------------------------
        maingroup.Update(GetFrameTime());

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);





              maingroup.Render();



            DrawRectangle(0, 0, screenWidth, 40, BLACK);
            DrawText(TextFormat("bunnies: %i", maingroup.GetGameObjectCount()), 120, 10, 20, GREEN);
            DrawText(TextFormat("batched draw calls: %i", 1 +  maingroup.GetGameObjectCount()/MAX_BATCH_ELEMENTS), 320, 10, 20, MAROON);

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    maingroup.ClearData();
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
