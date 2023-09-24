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

const int screenWidth = 800;
const int screenHeight = 450;

const int gravityY = 250;
const int jumpForce = -150;

Texture2D birdTexture;
Texture2D pipeTexture;

cpFloat mass = 1;
cpFloat radius = 5;
cpFloat timeStep = 1.0 / 60.0;
cpFloat moment;

cpVect gravity;
cpSpace *space;

cpShape *birdShape;
cpBody *birdBody;

cpVect birdPos;
cpVect birdVel;

cpShape *pipeShape;
cpBody *pipeBody;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void); // Draw one frame
static void UpdatePhysics(void);   // Make a step in the physics engine
static void HandleInput(void);     // Capture user input and do stuff

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main() {
  // Initialization
  //--------------------------------------------------------------------------------------

  InitWindow(screenWidth, screenHeight, "raylib");

  birdTexture = LoadTexture("resources/sprites/yellowbird-midflap.png");
  pipeTexture = LoadTexture("resources/sprites/pipe-green.png");

  gravity = cpv(0, gravityY);
  space = cpSpaceNew();
  cpSpaceSetGravity(space, gravity);

  moment = cpMomentForCircle(mass, 0, radius, cpvzero);

  birdBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
  cpBodySetPosition(birdBody, cpv(screenWidth / 3, screenHeight / 3));

  birdShape = cpSpaceAddShape(space, cpCircleShapeNew(birdBody, radius, cpvzero));
  cpShapeSetFriction(birdShape, 0.7);

  pipeBody = cpSpaceAddBody(space, cpBodyNewStatic());
  cpBodySetPosition(pipeBody, cpv(screenWidth / 3, screenHeight - 40));

  pipeShape = cpSpaceAddShape(
      space, cpBoxShapeNew(pipeBody, pipeTexture.width, pipeTexture.height / screenHeight - 40, 1));
  cpShapeSetFriction(pipeShape, 0.7);

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

  UnloadTexture(birdTexture);
  UnloadTexture(pipeTexture);

  cpShapeFree(birdShape);
  cpShapeFree(pipeShape);

  cpBodyFree(birdBody);
  cpBodyFree(pipeBody);

  cpSpaceFree(space);
  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

static void UpdatePhysics(void) {
  birdPos = cpBodyGetPosition(birdBody);
  birdVel = cpBodyGetVelocity(birdBody);
  // uncomment if you want to debug
  //   printf("Time is %5.2f. birdBody is at (%5.2f, %5.2f). It's birdVelocity is "
  //          "(%5.2f, %5.2f)\n",
  //          birdPos.x, birdPos.y, birdVel.x, birdVel.y);
  cpSpaceStep(space, timeStep);
}

static void HandleInput(void) {
  if (IsKeyDown(KEY_SPACE)) {
    cpBodySetVelocity(birdBody, cpv(0, jumpForce));
  }
}

// Update and draw game frame
static void UpdateDrawFrame(void) {
  // Draw, DONT PLACE ANY LOGIC HERE! Only draw.
  //----------------------------------------------------------------------------------
  BeginDrawing();

  ClearBackground(RAYWHITE);

  cpVect pipePos = cpBodyGetPosition(pipeBody);

  DrawTexture(birdTexture, birdPos.x, birdPos.y, WHITE);
  DrawTexture(pipeTexture, pipePos.x, pipePos.y, WHITE);

  // DrawText("This is a raylib example", 10, 40, 20, DARKGRAY);

  DrawFPS(10, 10);

  EndDrawing();
  //----------------------------------------------------------------------------------

  // Update Physics, Input and Logic
  // ---------------------------------------------------------------------------------
  UpdatePhysics();
  HandleInput();
}