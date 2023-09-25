#include <stdio.h>

#include <ferox.h>
#include <raylib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define TARGET_FPS 60

static const float CELL_SIZE = 4.0f, DELTA_TIME = 1.0f / TARGET_FPS;

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------

struct Pipe {
  int id;
  frBody *body;
};

const int screenWidth = 600;
const int screenHeight = 450;

const int gravityY = 250;
const int jumpForce = -150;

const int PIPES_COUNT = 8;

Texture2D birdTexture;
Texture2D pipeTexture;

float mass = 1;
float radius;
float timeStep = 1.0 / 60.0;
float moment;

frBody *birdBody;
frShape *birdShape;

static frWorld *world;

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

  world = frCreateWorld(frVector2ScalarMultiply(FR_WORLD_DEFAULT_GRAVITY, 20.0f), CELL_SIZE);

  for (int i = 0; i < PIPES_COUNT; i++) {
    int x_offset_factor = i;
    int yOffset = 0;

    if (i % 2 == 0) {
      yOffset = -(pipeTexture.height / 2);
    } else {
      yOffset = -(pipeTexture.height);
      x_offset_factor -= 1;
    }

    frBody *pipeBody = frCreateBodyFromShape(
        FR_BODY_KINEMATIC,
        frVector2PixelsToUnits(
            (frVector2){.x = screenWidth * 1.5 + pipeTexture.width + 100 * x_offset_factor,
                        .y = screenHeight + yOffset}),
        frCreateRectangle((frMaterial){.density = 1.0f, .friction = 0.35f},
                          frPixelsToUnits(pipeTexture.width), frPixelsToUnits(pipeTexture.height)));

    frAddBodyToWorld(world, pipeBody);
    frSetBodyVelocity(pipeBody, (frVector2){.x = -10.0f, .y = 0.0f});
  }

  birdBody = frCreateBodyFromShape(
      FR_BODY_DYNAMIC,
      frVector2PixelsToUnits((frVector2){.x = screenWidth / 2, .y = screenHeight / 3}),
      // frCreateCircle((frMaterial){.density = 10.0f, .friction = 2.0f}, radius));
      frCreateRectangle((frMaterial){.density = 1.0f, .friction = 0.35f},
                        frPixelsToUnits(birdTexture.width), frPixelsToUnits(birdTexture.height)));

  frAddBodyToWorld(world, birdBody);

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

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

static void HandleInput(void) {
  if (IsKeyDown(KEY_SPACE)) {
    frSetBodyVelocity(birdBody, (frVector2){.x = 0, .y = jumpForce});
  }
}

// Update and draw game frame
static void UpdateDrawFrame(void) {
  // Draw, DONT PLACE ANY LOGIC HERE! Only draw.
  //----------------------------------------------------------------------------------
  BeginDrawing();

  ClearBackground(RAYWHITE);

  frVector2 birdPos = frGetBodyPosition(birdBody);

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

static void UpdatePhysics(void) {

  for (int i = 0; i < PIPES_COUNT; i++) {
    frBody *pipeBody = frGetBodyFromWorld(world, i);
    frVector2 pipePos = frVector2UnitsToPixels(frGetBodyPosition(pipeBody));

    if (pipePos.x <= 0) {
      frSetBodyPosition(pipeBody, (frVector2){.x = screenWidth, .y = pipePos.y});
    }
  }

  frUpdateWorld(world, DELTA_TIME);
}

static void DrawPipes(void) {
  for (int i = 0; i < PIPES_COUNT; i++) {
    frBody *pipeBody = frGetBodyFromWorld(world, i);
    frVector2 pipePos = frVector2UnitsToPixels(frGetBodyPosition(pipeBody));

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
        (Vector2){.x = pipeTexture.width / 2, .y = pipeTexture.height / 2}, 0, WHITE);
  }
}