#include <stdio.h>

#include <chipmunk/chipmunk.h>
#include <chipmunk/chipmunk_structs.h>
#include <raylib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
Texture2D bird_texture;

cpFloat mass = 1;
cpFloat radius = 5;
cpFloat timeStep = 1.0 / 60.0;
cpFloat moment;

cpVect gravity;
cpSpace *space;

cpBody *birdBody;
cpVect pos;
cpVect vel;
//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void); // Update and draw one frame

const int screenWidth = 800;
const int screenHeight = 450;

const int gravityY = 250;

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main() {
  // Initialization
  //--------------------------------------------------------------------------------------

  gravity = cpv(0, gravityY);
  space = cpSpaceNew();
  cpSpaceSetGravity(space, gravity);

  moment = cpMomentForCircle(mass, 0, radius, cpvzero);

  birdBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
  cpBodySetPosition(birdBody, cpv(screenWidth / 3, screenHeight / 3));

  InitWindow(screenWidth, screenHeight, "raylib");
  bird_texture = LoadTexture("resources/sprites/yellowbird-midflap.png");

  //--------------------------------------------------------------------------------------

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    UpdateDrawFrame();
  }
#endif

  UnloadTexture(bird_texture);

  cpBodyFree(birdBody);
  cpSpaceFree(space);
  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

static void UpdatePhysics(void) {
  pos = cpBodyGetPosition(birdBody);
  vel = cpBodyGetVelocity(birdBody);
  // uncomment if you want to debug
  //   printf("Time is %5.2f. birdBody is at (%5.2f, %5.2f). It's velocity is "
  //          "(%5.2f, %5.2f)\n",
  //          pos.x, pos.y, vel.x, vel.y);

  cpSpaceStep(space, timeStep);
}

static void HandleInput(void) {
  if (IsKeyDown(KEY_SPACE)) {
    cpBodySetVelocity(birdBody, cpv(0, -150));
  }
}

// Update and draw game frame
static void UpdateDrawFrame(void) {
  // Draw, DONT PLACE ANY LOGIC HERE! Only draw.
  //----------------------------------------------------------------------------------
  BeginDrawing();

  ClearBackground(RAYWHITE);

  DrawTexture(bird_texture, pos.x, pos.y, WHITE);

  // DrawText("This is a raylib example", 10, 40, 20, DARKGRAY);

  DrawFPS(10, 10);

  EndDrawing();
  //----------------------------------------------------------------------------------

  // Update Physics, Input and Logic
  // ---------------------------------------------------------------------------------
  UpdatePhysics();
  HandleInput();
}