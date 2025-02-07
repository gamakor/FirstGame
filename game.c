// #define RAYMATH_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

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

typedef enum State
{
    StateMainMenu,
    StateRunning,
    StateGameOver
} State;

typedef struct GameState
{
    /* data */
    State state;
    float gameTime;
    Vector2 playerPos;
    float playerSize;
    float playerSpeed;
    float shootDelay;
    float shootTime;
    bool canShoot;
    float projSpeed;
    float projSize;
    int projPierce;
    int killCounter;

    int projectileCount;
    Projectile projectiles[MAX_PROJECTILES];

    float spawnTime;

    int enemyCount;
    Enemy Enemies[MAX_ENEMIES];

} GameState;

static const GameState DefaultState = {

    .state = StateMainMenu,
    .playerPos = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f},
    .playerSize = 20.0f,
    .playerSpeed = 50.0f,
    .shootDelay = .3f,
    .shootTime = .0f,
    .projSpeed = 100,
    .projSize = 4,

};

void draw_text_centered(const char *text, Vector2 pos, float fontSize)
{
    const Vector2 text_size = MeasureTextEx(GetFontDefault(), text, fontSize, 1);
    pos.x -= text_size.x / 2.0f;
    DrawText(text, pos.x, pos.y, fontSize, WHITE);
}

void GameUpdate(GameState *gameState)
{

    gameState->gameTime += GetFrameTime();
    // Draw Kill counter
    const char killCounterText[100] = {0};
    sprintf(killCounterText, "KillCounter: %d", gameState->killCounter);
    draw_text_centered(killCounterText, (Vector2){80, 30}, 20);

    // shoot projectile
    {
        if (gameState->shootTime < gameState->shootDelay)
        {
            gameState->shootTime += GetFrameTime();
        }

        while (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && gameState->shootTime >= gameState->shootDelay)
        {
            if (gameState->projectileCount >= MAX_PROJECTILES)
            {
                break;
            }

            Vector2 dir = Vector2Normalize(Vector2Subtract(GetMousePosition(), gameState->playerPos));

            Projectile proj =
                {
                    .pos = gameState->playerPos,
                    .dir = dir,
                    .size = gameState->projSize,
                    .speed = gameState->projSpeed,
                    .pierce = gameState->projPierce};
            gameState->projectiles[gameState->projectileCount++] = proj;

            gameState->shootTime -= gameState->shootDelay;
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
            gameState->playerPos.x -= gameState->playerSpeed * GetFrameTime();
        }
        if (IsKeyDown(KEY_D))
        {
            gameState->playerPos.x += gameState->playerSpeed * GetFrameTime();
        }
        if (IsKeyDown(KEY_S))
        {
            gameState->playerPos.y += gameState->playerSpeed * GetFrameTime();
        }
        if (IsKeyDown(KEY_W))
        {
            gameState->playerPos.y -= gameState->playerSpeed * GetFrameTime();
        }

        DrawCircle(gameState->playerPos.x, gameState->playerPos.y, gameState->playerSize, GREEN);
    }

    // Update projectiles
    {
        for (int projIdx = 0; projIdx < gameState->projectileCount; projIdx++)
        {
            Projectile *proj = &gameState->projectiles[projIdx];

            proj->pos.x += proj->dir.x * proj->speed * GetFrameTime();
            proj->pos.y += proj->dir.y * proj->speed * GetFrameTime();

            const Rectangle screenRect ={
                .height = SCREEN_HEIGHT,
                .width = SCREEN_WIDTH,
                .x = 0, 
                .y = 0
            };

            if(!CheckCollisionPointRec(proj->pos, screenRect))
            {
                // replace with the last project tile
                *proj = gameState->projectiles[--gameState->projectileCount];
            }

            DrawCircle(proj->pos.x, proj->pos.y, proj->size, YELLOW);
            // collide with enemies
            for (int enemyIdx = 0; enemyIdx < gameState->enemyCount; enemyIdx++)
            {
                Enemy *enemy = &gameState->Enemies[enemyIdx];

                if (CheckCollisionCircles(proj->pos, proj->size, enemy->pos, enemy->size))
                {
                    // Replace with last enemy
                    *enemy = gameState->Enemies[--gameState->enemyCount];
                    proj->pierce--;
                    gameState->killCounter++;

                    // scaling

                    if (gameState->killCounter % 10 == 0)
                    {
                        gameState->projSpeed += 10;
                    }
                    if (gameState->killCounter % 15 == 0)
                    {
                        gameState->projSize += 1;
                    }
                    if (gameState->killCounter % 20 == 0)
                    {
                        gameState->projPierce += 1;
                    }

                    if (proj->pierce < 0)
                    {
                        // replace with the last project tile
                        *proj = gameState->projectiles[--gameState->projectileCount];
                    }
                    break;
                }
            }
        }
    }

    // Update Enemies
    {
        // Spawn Enemies
        const float spawnFrequency = 1 / (gameState->gameTime * .1f);
        const float enemySize = 8 + gameState->gameTime / 20.0f;
        const float enemySpeed = 40 + gameState->gameTime / 10.0f;
        gameState->spawnTime += GetFrameTime();

        while (gameState->spawnTime >= spawnFrequency)
        {
            if (gameState->enemyCount >= MAX_ENEMIES)
            {
                break;
            }

            float radians = GetRandomValue(0, 360) * PI / 180.0f;
            Vector2 dir = {cosf(radians), sinf(radians)};
            const float dist = SCREEN_WIDTH / 2.0f;

            Enemy enemy =
                {
                    .pos = {gameState->playerPos.x + dir.x * dist, gameState->playerPos.y + dir.y * dist},
                    .size = enemySize,
                };

            gameState->Enemies[gameState->enemyCount++] = enemy;

            gameState->spawnTime -= spawnFrequency;
        }

        // update enemies

        for (int enemyIdx = 0; enemyIdx < gameState->enemyCount; enemyIdx++)
        {
            Enemy *enemy = &gameState->Enemies[enemyIdx];

            Vector2 dir = Vector2Normalize(Vector2Subtract(gameState->playerPos, enemy->pos));

            enemy->pos.x += dir.x * enemySpeed * GetFrameTime();
            enemy->pos.y += dir.y * enemySpeed * GetFrameTime();

            // draw enemy
            DrawCircle(enemy->pos.x, enemy->pos.y, enemySize, RED);

            // Touch player to restart the game
            if (CheckCollisionCircles(enemy->pos, enemy->size, gameState->playerPos, gameState->playerSize))
            {

                gameState->playerSize -= 2;
                if (gameState->playerSize <= 0)
                {
                    // Need to change this to the game over state.
                    gameState->state = StateGameOver;
                }
                *enemy = gameState->Enemies[--gameState->enemyCount];
            }
        }

        // const int texture_x = SCREEN_WIDTH / 2 - texture.width / 2;
        // const int texture_y = SCREEN_HEIGHT / 2 - texture.height / 2;
        // DrawTexture(texture, texture_x, texture_y, WHITE);

        // const char* text = "OMG! IT WORKS!";
        // const Vector2 text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
        // DrawText(text, SCREEN_WIDTH / 2 - text_size.x / 2, texture_y + texture.height + text_size.y + 10, 20, BLACK);
    }
    EndDrawing();
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    GameState gameState = DefaultState;

    //    Texture2D texture = LoadTexture("assets/test.png"); // Check README.md for how this works

    while (!WindowShouldClose())
    {
        // gameState.gameTime += GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);

        switch (gameState.state)
        {
        case StateMainMenu:
            
            draw_text_centered("Bullet Hell", (Vector2){SCREEN_WIDTH/2.0f, 40.0f}, 80);
            draw_text_centered("Press Enter to Start", (Vector2){SCREEN_WIDTH/2.0f, 200.0f}, 20);

            if(IsKeyDown(KEY_ENTER))
            {
                gameState.state = StateRunning;
            }
            EndDrawing();
            break;

        case StateRunning:
            GameUpdate(&gameState);

            break;
        case StateGameOver:
            /* code */
            draw_text_centered("Game Over", (Vector2){SCREEN_WIDTH/2.0f, 40.0f}, 80);
            draw_text_centered("Press Enter to Restart", (Vector2){SCREEN_WIDTH/2.0f, 200.0f}, 20);

            const char killCounterText[100] = {0};
            sprintf(killCounterText, "KillCounter: %d", gameState.killCounter);
            draw_text_centered(killCounterText, (Vector2){80, 30}, 20);

            if(IsKeyDown(KEY_ENTER))
            {
                gameState = DefaultState;
                gameState.state = StateRunning;
            }
            EndDrawing();
            break;
        default:
            break;
        }
    }
    CloseWindow();

    return 0;
}