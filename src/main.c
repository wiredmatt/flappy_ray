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

struct Pipe {
  int id;
  cpBody *body;
  cpShape *shape;
};

const int screenWidth = 600;
const int screenHeight = 450;

const int gravityY = 250;
const int jumpForce = -150;

const int PIPES_COUNT = 10;

Texture2D birdTexture;
Texture2D pipeTexture;

cpFloat mass = 1;
cpFloat radius;
cpFloat timeStep = 1.0 / 60.0;
cpFloat moment;

cpVect gravity;
cpSpace *space;

cpShape *birdShape;
cpBody *birdBody;

cpVect birdPos;
cpVect birdVel;

struct Pipe pipes[] = {}; // 8 pipes in total (2 up and 2 down)

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void); // Draw one frame
static void UpdatePhysics(void);   // Make a step in the physics engine
static void HandleInput(void);     // Capture user input and do stuff
static void DrawPipes(void);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main() {
  // Initialization
  //--------------------------------------------------------------------------------------

  InitWindow(screenWidth, screenHeight, "raylib");

  birdTexture = LoadTexture("resources/sprites/yellowbird-midflap.png");
  pipeTexture = LoadTexture("resources/sprites/pipe-green.png");

  radius = birdTexture.height / 2; // TODO: Change player collision to use polygonshape instead

  gravity = cpv(0, gravityY);
  space = cpSpaceNew();
  cpSpaceSetGravity(space, gravity);

  moment = cpMomentForCircle(mass, 0, radius, cpvzero);

  birdBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
  cpBodySetPosition(birdBody, cpv(screenWidth / 3, screenHeight / 3));

  birdShape = cpSpaceAddShape(space, cpCircleShapeNew(birdBody, radius, cpvzero));
  cpShapeSetFriction(birdShape, 0.7);

  for (int i = 0; i < PIPES_COUNT; i++) {
    int x_offset_factor = i;
    int yOffset = 0;

    if (i % 2 == 0) {
      yOffset = -(pipeTexture.height / 2);
    } else {
      yOffset = -(pipeTexture.height);
      x_offset_factor -= 1;
    }

    cpBody *pipeBody = cpSpaceAddBody(space, cpBodyNewKinematic());
    cpBodySetPosition(pipeBody, cpv(screenWidth * 1.5 + pipeTexture.width - 100 * x_offset_factor,
                                    screenHeight + yOffset));

    cpShape *pipeShape =
        cpSpaceAddShape(space, cpBoxShapeNew(pipeBody, pipeTexture.width, pipeTexture.height, 1));

    cpShapeSetFriction(pipeShape, 0.7);

    cpBodySetVelocity(pipeBody, cpv(-200, 0));

    pipes[i].id = i;
    pipes[i].body = pipeBody;
    pipes[i].shape = pipeShape;
  }

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
  // cpShapeFree(pipeShape);

  cpBodyFree(birdBody);
  // cpBodyFree(pipeBody);

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

  for (int i = 0; i < PIPES_COUNT; i++) {
    cpVect pipePos = cpBodyGetPosition(pipes[i].body);
    int x_offset_factor = 0;

    if (i % 2 != 0) {
      x_offset_factor -= 1;
    }

    if (pipePos.x <= 0) {
      cpBodySetPosition(pipes[i].body, cpv(screenWidth, pipePos.y));
    }
  }

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

  DrawTexture(birdTexture, birdPos.x, birdPos.y, WHITE);

  DrawPipes();
  // DrawTexture(pipeTexture, pipePos.x, pipePos.y, WHITE);

  // DrawText("This is a raylib example", 10, 40, 20, DARKGRAY);

  DrawFPS(10, 10);

  EndDrawing();
  //----------------------------------------------------------------------------------

  // Update Physics, Input and Logic
  // ---------------------------------------------------------------------------------
  UpdatePhysics();
  // UpdatePipes();
  HandleInput();
}

static void DrawPipes(void) {
  for (int i = 0; i < PIPES_COUNT; i++) {
    cpVect pipePos = cpBodyGetPosition(pipes[i].body);
    int rotation = 0;
    Vector2 origin = (Vector2){.x = 0, .y = 0};

    if (i % 2 == 0) {
      rotation = 0;
    } else {
      rotation = 180;
      origin = (Vector2){.x = pipeTexture.width, .y = 0};
    }

    DrawTexturePro(
        pipeTexture,
        (Rectangle){.x = 0, .y = 0, .width = pipeTexture.width, .height = pipeTexture.height},
        (Rectangle){.x = pipePos.x,
                    .y = pipePos.y,
                    .width = pipeTexture.width,
                    .height = pipeTexture.height},
        origin, rotation, WHITE);
  }
}