/*
    Based off: https://github.com/c-krit/ferox/blob/main/examples/src/basic.c
    By: Jaedeok Kim <jdeokkim@protonmail.com>
*/

/* Includes ============================================================================= */

#include "ferox.h"
#include "raylib.h"
#include "raymath.h"
#include "stdio.h"

#define FEROX_RAYLIB_IMPLEMENTATION
#include "ferox-raylib.h"

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

/* Macros =============================================================================== */

#define TARGET_FPS 60
#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 600
#define PIPES_QTY 8
#define SPACE_BETWEEN_PIPES 120
#define PIPE_VELOCITY -12.0f
#define JUMP_FORCE -20.0f

/* Constants ============================================================================ */

static const float CELL_SIZE = 4.0f, DELTA_TIME = 1.0f / TARGET_FPS;

/* Private Variables ==================================================================== */

static frWorld *world;

static frBody *birdBody, *groundBody;

static Rectangle bounds = {.width = SCREEN_WIDTH, .height = SCREEN_HEIGHT};

static int lowestLowerY;
static int highestLowerY;

static Texture2D pipeTexture, birdTexture, backgroundDayTexture, groundTexture;

bool alive = true;

/* Private Function Prototypes ========================================================== */

static void InitGame(void);
static void UpdateGame(void);
static void DeInitGame(void);
static void DrawPipes(void);
static void UpdatePipes(void);
static void HandleCollision(void);
// static bool BirdCollisionBegin(frBodyPair key, const frCollision *value);
// static bool BirdCollisionEnd(frBodyPair key, const frCollision *value);

/* Public Functions ===================================================================== */

int randomYOffset() { return (rand() % (lowestLowerY - highestLowerY + 1)) + highestLowerY; }

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "c-krit/ferox | basic.c");

  InitGame();

#ifdef PLATFORM_WEB
  emscripten_set_main_loop(UpdateGame, 0, 1);
#else
  SetTargetFPS(TARGET_FPS);

  while (!WindowShouldClose())
    UpdateGame();
#endif

  DeInitGame();

  CloseWindow();

  return 0;
}

/* Private Functions ==================================================================== */

static void InitGame(void) {
  /**
   * Load the sprites to use
   */
  pipeTexture = LoadTexture("resources/sprites/pipe-green.png");
  birdTexture = LoadTexture("resources/sprites/yellowbird-midflap.png");
  backgroundDayTexture = LoadTexture("resources/sprites/background-day.png");
  groundTexture = LoadTexture("resources/sprites/base.png");

  /**
   * Define the range in which the lower pipes can be generated
   * "highest" is the closest to y=0
   * "lowest" is the closest to y=SCREEN_HEIGHT
   * -------x- y = 0
   * |       |
   * |       |
   * |       |
   * |      x| y = HIGHEST LOWER -> highest Y position in which the pipe can be generated
   * |       |
   * |       |
   * |       |
   * |      x| y = LOWEST LOWER -> lowest Y position in which the pipe can be generated
   * |       |
   * |------x- y = SCREEN_HEIGHT
   */
  highestLowerY = pipeTexture.height + 120;
  lowestLowerY = SCREEN_HEIGHT - pipeTexture.height;

  // To simulate physics, a `world` needs to be created.
  // It requires `gravity` and a `cell size`, the later being used to compute collisions
  // with more precission
  world = frCreateWorld(frVector2ScalarMultiply(FR_WORLD_DEFAULT_GRAVITY, 5.0f), CELL_SIZE);

  int yOffset;

  for (int i = 0; i < PIPES_QTY; i++) {
    int xOffsetFactor = i;

    if (i % 2 == 0) { // LOWER PIPES
      yOffset = randomYOffset();
    } else { // UPPER PIPES
      yOffset = yOffset - pipeTexture.height - SPACE_BETWEEN_PIPES;
      xOffsetFactor -= 1;
    }

    frBody *pipeBody = frCreateBodyFromShape(
        FR_BODY_KINEMATIC, // It collides but doesn't fall to gravity.
        frVector2PixelsToUnits((frVector2){
            .x = SCREEN_WIDTH * 1.25 + pipeTexture.width + 100 * xOffsetFactor, .y = yOffset}),
        frCreateRectangle((frMaterial){.density = 1.0f, .friction = 0.35f},
                          frPixelsToUnits(pipeTexture.width), frPixelsToUnits(pipeTexture.height)));

    frSetBodyVelocity(pipeBody, (frVector2){.x = PIPE_VELOCITY, .y = 0.0f});
    frAddBodyToWorld(world, pipeBody);
  }

  birdBody = frCreateBodyFromShape(
      FR_BODY_DYNAMIC, // It collides and falls to gravity.
      frVector2PixelsToUnits((frVector2){.x = 0.5f * SCREEN_WIDTH, .y = 0.35f * SCREEN_HEIGHT}),
      frCreateCircle((frMaterial){.density = 1.0f, .friction = 0.35f}, birdTexture.height / 14));

  groundBody = frCreateBodyFromShape(
      FR_BODY_STATIC, // It collides
      frVector2PixelsToUnits((frVector2){.x = 0, .y = SCREEN_HEIGHT - groundTexture.height / 2}),
      frCreateRectangle((frMaterial){.density = 1.0f, .friction = 0.35f},
                        frPixelsToUnits(SCREEN_WIDTH), frPixelsToUnits(1.0f)));

  frAddBodyToWorld(world, birdBody);
  frAddBodyToWorld(world, groundBody);

  // NOTE: This should work but it's not? Should ask https://github.com/jdeokkim about it.
  // frSetWorldCollisionHandler(
  //     world, (frCollisionHandler){.preStep = BirdCollisionBegin, .postStep = BirdCollisionEnd});
}

// static bool BirdCollisionBegin(frBodyPair key, const frCollision *value) {
//   printf("Bird collided with something!\n");

//   return true;
// }

// static bool BirdCollisionEnd(frBodyPair key, const frCollision *value) {
//   printf("Bird collided with something!\n");

//   return true;
// }

static void HandleInput(void) {
  if (IsKeyPressed(KEY_SPACE)) {
    frSetBodyVelocity(birdBody, (frVector2){.x = 0, .y = JUMP_FORCE});
  }
}

static void HandleCollision(void) {
  frCollision collWPipes;
  frCollision collWGround;

  frComputeCollision(frGetBodyShape(birdBody), frGetBodyTransform(birdBody),
                     frGetBodyShape(groundBody), frGetBodyTransform(groundBody), &collWGround);

  for (int i = 0; i < PIPES_QTY; i++) {
    frBody *pipeBody = frGetBodyFromWorld(world, i);

    frComputeCollision(frGetBodyShape(birdBody), frGetBodyTransform(birdBody),
                       frGetBodyShape(pipeBody), frGetBodyTransform(pipeBody), &collWPipes);
  }

  if (collWGround.count > 0 || collWPipes.count > 0) {
    frSetBodyVelocity(birdBody, (frVector2){.x = 0, .y = 0});
    printf("Bird collided with something!\n");
    // TODO: Handle your end state here!
  }
}

static void UpdateGame(void) {
  frUpdateWorld(world, DELTA_TIME);
  frVector2 birdPos = frVector2UnitsToPixels(frGetBodyPosition(birdBody));
  HandleInput();

  UpdatePipes();
  HandleCollision();

  {
    BeginDrawing();

    ClearBackground(FR_DRAW_COLOR_MATTEBLACK);

    DrawTextureEx(backgroundDayTexture, (Vector2){0, 0}, 0.0f, 1.4f, WHITE);

    DrawPipes();

    DrawTextureEx(groundTexture, (Vector2){0, SCREEN_HEIGHT - groundTexture.height / 2}, 0.0f, 1.4f,
                  WHITE);

    // frDrawGrid(bounds, CELL_SIZE, 0.25f, ColorAlpha(DARKGRAY, 0.75f)); -> debug with cells

    // Draw player with collision shape in blue
    // frDrawBodyLines(birdBody, 1.0f, BLUE);
    // some offsets to match the body shape
    DrawTexture(birdTexture, birdPos.x - 15, birdPos.y - 10, WHITE);

    DrawFPS(8, 8);

    EndDrawing();
  }
}

static void DrawPipes(void) {
  for (int i = 0; i < PIPES_QTY; i++) {
    frBody *pipeBody = frGetBodyFromWorld(world, i);
    frVector2 pipePos = frVector2UnitsToPixels(frGetBodyPosition(pipeBody));

    int rotation = 0;

    if (i % 2 == 0) {
      rotation = 0;
    } else {
      rotation = 180;
    }

    // frDrawBodyLines(pipeBody, 3.0f, WHITE);
    DrawTexturePro(
        pipeTexture,
        (Rectangle){.x = 0, .y = 0, .width = pipeTexture.width, .height = pipeTexture.height},
        (Rectangle){.x = pipePos.x,
                    .y = pipePos.y,
                    .width = pipeTexture.width,
                    .height = pipeTexture.height},
        (Vector2){.x = pipeTexture.width / 2, .y = pipeTexture.height / 2}, rotation, WHITE);
  }
}

static void UpdatePipes(void) {
  int yOffset;

  for (int i = 0; i < PIPES_QTY; i++) {
    if (i % 2 == 0) { // LOWER PIPES
      yOffset = randomYOffset();
    } else { // UPPER PIPES
      yOffset = yOffset - pipeTexture.height - SPACE_BETWEEN_PIPES;
    }

    frBody *pipeBody = frGetBodyFromWorld(world, i);
    frVector2 pipePos = frVector2UnitsToPixels(frGetBodyPosition(pipeBody));

    if (pipePos.x <= -pipeTexture.width) {
      frSetBodyPosition(pipeBody, frVector2PixelsToUnits((frVector2){
                                      .x = SCREEN_WIDTH * 1.75 + pipeTexture.width, .y = yOffset}));
    }
  }
}

static void DeInitGame(void) {
  /**
   * Ferox stuff - No need to release bodies.
   */
  frReleaseShape(frGetBodyShape(birdBody));
  frReleaseShape(frGetBodyShape(groundBody));
  for (int i = 0; i < PIPES_QTY; i++) {
    frBody *pipeBody = frGetBodyFromWorld(world, i);
    frReleaseShape(frGetBodyShape(pipeBody));
  }
  frReleaseWorld(world);

  /**
   * RayLib stuff
   */
  UnloadTexture(pipeTexture);
  UnloadTexture(birdTexture);
  UnloadTexture(groundTexture);
  UnloadTexture(backgroundDayTexture);
}