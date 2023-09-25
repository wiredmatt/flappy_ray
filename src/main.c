/*
    Copyright (c) 2021-2023 Jaedeok Kim <jdeokkim@protonmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

/* Includes ============================================================================= */

#include "ferox.h"
#include "raylib.h"
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
#define PIPE_BASE_VEL -20.0f

/* Constants ============================================================================ */

static const float CELL_SIZE = 4.0f, DELTA_TIME = 1.0f / TARGET_FPS;

/* Private Variables ==================================================================== */

static frWorld *world;

static frBody *box, *ground;

static Texture2D pipeTexture, birdTexture;

static Rectangle bounds = {.width = SCREEN_WIDTH, .height = SCREEN_HEIGHT};

/* Private Function Prototypes ========================================================== */

static void InitExample(void);
static void UpdateExample(void);
static void DeinitExample(void);
static void DrawPipes(void);
static void UpdatePipes(void);

/* Public Functions ===================================================================== */

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "c-krit/ferox | basic.c");

  InitExample();

#ifdef PLATFORM_WEB
  emscripten_set_main_loop(UpdateExample, 0, 1);
#else
  SetTargetFPS(TARGET_FPS);

  while (!WindowShouldClose())
    UpdateExample();
#endif

  DeinitExample();

  CloseWindow();

  return 0;
}

/* Private Functions ==================================================================== */

static void InitExample(void) {
  pipeTexture = LoadTexture("resources/sprites/pipe-green.png");
  birdTexture = LoadTexture("resources/sprites/yellowbird-midflap.png");

  world = frCreateWorld(frVector2ScalarMultiply(FR_WORLD_DEFAULT_GRAVITY, 4.0f), CELL_SIZE);

  ground = frCreateBodyFromShape(
      FR_BODY_STATIC,
      frVector2PixelsToUnits((frVector2){.x = 0.5f * SCREEN_WIDTH, .y = 0.85f * SCREEN_HEIGHT}),
      frCreateRectangle((frMaterial){.density = 1.25f, .friction = 0.5f},
                        frPixelsToUnits(0.75f * SCREEN_WIDTH),
                        frPixelsToUnits(0.1f * SCREEN_HEIGHT)));

  for (int i = 0; i < PIPES_QTY; i++) {
    int x_offset_factor = i;
    int yOffset = 0;

    if (i % 2 == 0) {
      yOffset = -(pipeTexture.height / 4);
    } else {
      yOffset = -(pipeTexture.height) * 2;
      x_offset_factor -= 1;
    }

    frBody *movingPipe = frCreateBodyFromShape(
        FR_BODY_KINEMATIC,
        frVector2PixelsToUnits(
            (frVector2){.x = SCREEN_WIDTH * 1.5 + pipeTexture.width + 100 * x_offset_factor,
                        .y = SCREEN_HEIGHT + yOffset}),
        frCreateRectangle((frMaterial){.density = 1.0f, .friction = 0.35f},
                          frPixelsToUnits(pipeTexture.width), frPixelsToUnits(pipeTexture.height)));

    frSetBodyVelocity(movingPipe, (frVector2){.x = PIPE_BASE_VEL, .y = 0.0f});
    frAddBodyToWorld(world, movingPipe);
  }

  box = frCreateBodyFromShape(
      FR_BODY_DYNAMIC,
      frVector2PixelsToUnits((frVector2){.x = 0.5f * SCREEN_WIDTH, .y = 0.35f * SCREEN_HEIGHT}),
      frCreateCircle((frMaterial){.density = 1.0f, .friction = 0.35f}, birdTexture.height / 14));
  // frCreateRectangle((frMaterial){.density = 1.0f, .friction = 0.35f}, frPixelsToUnits(45.0f),
  //                   frPixelsToUnits(45.0f)));

  frAddBodyToWorld(world, box);
  frAddBodyToWorld(world, ground);
}

static void HandleInput(void) {
  if (IsKeyDown(KEY_SPACE)) {
    frSetBodyVelocity(box, (frVector2){.x = 0, .y = -15.0f});
  }
}

static void UpdateExample(void) {
  frUpdateWorld(world, DELTA_TIME);
  frVector2 birdPos = frVector2UnitsToPixels(frGetBodyPosition(box));
  HandleInput();
  UpdatePipes();

  {
    BeginDrawing();

    ClearBackground(FR_DRAW_COLOR_MATTEBLACK);

    frDrawGrid(bounds, CELL_SIZE, 0.25f, ColorAlpha(DARKGRAY, 0.75f));

    frDrawBodyLines(ground, 1.0f, GRAY);

    frDrawBodyLines(box, 1.0f, BLUE);
    DrawTexture(birdTexture, birdPos.x - 15, birdPos.y - 10, WHITE);

    DrawPipes();

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

    frDrawBodyLines(pipeBody, 3.0f, WHITE);
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
  for (int i = 0; i < PIPES_QTY; i++) {
    frBody *pipeBody = frGetBodyFromWorld(world, i);
    frVector2 pipePos = frVector2UnitsToPixels(frGetBodyPosition(pipeBody));

    if (pipePos.x <= 0) {
      frSetBodyPosition(pipeBody,
                        frVector2PixelsToUnits((frVector2){
                            .x = SCREEN_WIDTH * 1.5 + pipeTexture.width + 100, .y = pipePos.y}));
    }
  }
}

static void DeinitExample(void) {
  frReleaseShape(frGetBodyShape(ground));
  frReleaseShape(frGetBodyShape(box));

  frReleaseWorld(world);

  UnloadTexture(pipeTexture);
}