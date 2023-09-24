// #include "../build/_deps/chipmunk-src/include/chipmunk/chipmunk.h"
#include <chipmunk.h>
#include <flecs.h>
#include <raylib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
Texture2D bird_texture;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);   // Update and draw one frame

const int screenWidth = 800;
const int screenHeight = 450;

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main() {
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, "raylib");
    bird_texture = LoadTexture("resources/sprites/yellowbird-midflap.png");

    //--------------------------------------------------------------------------------------

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);	// Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())   // Detect window close button or ESC key
    {
	UpdateDrawFrame();
    }
#endif

    UnloadTexture(bird_texture);

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();   // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Update and draw game frame
static void UpdateDrawFrame(void) {
    // Update
    // ---------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawTexture(bird_texture, 100, 100, WHITE);

    // DrawText("This is a raylib example", 10, 40, 20, DARKGRAY);

    DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}