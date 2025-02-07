//#define RAYMATH_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)
#define MAX_PROJECTILES (100)
#define MAX_ENEMIES (100)

#define WINDOW_TITLE "Window title"

typedef struct Projectile
{
    Vector2 pos;
    Vector2 dir;
    float size;
    float speed;
    int pierce;

} Projectile;

typedef struct Enemy
{
    Vector2 pos;
    float size;
} Enemy;

typedef struct GameState
{
    /* data */
    Vector2 playerPos;
    float playerSize;
    float playerSpeed;
    float shootDelay;
    float shootTime;
    bool canShoot;
    float projSpeed;
    float projSize;
    int projPierce;

    int projectileCount;
    Projectile projectiles[MAX_PROJECTILES];

    int enemyCount;
    Enemy Enemies[MAX_ENEMIES];

} GameState;

static const GameState DefaultState = {
    .playerPos = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f},
    .playerSize = 20.0f,
    .playerSpeed = 50.0f,
    .shootDelay = .3f,
    .shootTime = .0f,
    .projSpeed = 100,
    .projSize = 4,

};

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    GameState gameState = DefaultState;

//    Texture2D texture = LoadTexture("assets/test.png"); // Check README.md for how this works

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        // shoot projectile
        {
            if (gameState.shootTime < gameState.shootDelay)
            {
                gameState.shootTime += GetFrameTime();
            }

            while (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && gameState.shootTime >= gameState.shootDelay)
            {
                if (gameState.projectileCount >= MAX_PROJECTILES)
                {
                    break;
                }

                Vector2 dir = Vector2Normalize(Vector2Subtract( GetMousePosition(),gameState.playerPos));

                Projectile proj =
                    {
                        .pos = gameState.playerPos,
                        .dir = dir,
                        .size = gameState.projSize,
                        .speed = gameState.projSpeed
                    };
                gameState.projectiles[gameState.projectileCount++] = proj;

                gameState.shootTime -= gameState.shootDelay;
            }
        }

       
        {
            typedef struct Projectile
            {
                Vector2 pos;
                Vector2 dir;
                float size;
                float speed;
                int pierce;

            } Projectile;

            typedef struct Enemy
            {
                Vector2 pos;
                float size;
            } Enemy;

             // Move the Player

            if (IsKeyDown(KEY_A))
            {
                gameState.playerPos.x -= gameState.playerSpeed * GetFrameTime();
            }
            if (IsKeyDown(KEY_D))
            {
                gameState.playerPos.x += gameState.playerSpeed * GetFrameTime();
            }
            if (IsKeyDown(KEY_S))
            {
                gameState.playerPos.y += gameState.playerSpeed * GetFrameTime();
            }
            if (IsKeyDown(KEY_W))
            {
                gameState.playerPos.y -= gameState.playerSpeed * GetFrameTime();
            }

            DrawCircle(gameState.playerPos.x, gameState.playerPos.y, gameState.playerSize, GREEN);
        }

        // Update projectiles
        {
            for (int projIdx = 0; projIdx < gameState.projectileCount; projIdx++)
            {
                Projectile *proj = &gameState.projectiles[projIdx];

                proj->pos.x += proj->dir.x * proj->speed * GetFrameTime();
                proj->pos.y += proj->dir.y * proj->speed * GetFrameTime();

                DrawCircle(proj->pos.x, proj->pos.y, proj->size, YELLOW);
            }
        }

        // const int texture_x = SCREEN_WIDTH / 2 - texture.width / 2;
        // const int texture_y = SCREEN_HEIGHT / 2 - texture.height / 2;
        // DrawTexture(texture, texture_x, texture_y, WHITE);

        // const char* text = "OMG! IT WORKS!";
        // const Vector2 text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
        // DrawText(text, SCREEN_WIDTH / 2 - text_size.x / 2, texture_y + texture.height + text_size.y + 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}