#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

#define SCREEN_WIDTH  900
#define SCREEN_HEIGHT 900

#define BOARD_COLS 9
#define BOARD_ROWS 9

#define CELL_WIDTH ((SCREEN_WIDTH / BOARD_COLS))
#define CELL_HEIGHT ((SCREEN_HEIGHT / BOARD_ROWS))

// ---------------
// structures

typedef enum {
  V_NONE = 0,
  V_1,
  V_2,
  V_3,
  V_4,
  V_5,
  V_6,
  V_7,
  V_8,
  V_9,
  V_INVALID,  
} Value;

typedef struct {
  size_t x;
  size_t y;
} Pos;

typedef struct {
  size_t cols;
  size_t rows;
  Value *grid;
  
  Pos select;
} Game;

// ---------------
// headers

Game game_init(size_t cols, size_t rows);
void game_close(Game g);
Game game_set_selection(Game g, Pos pos);
void game_set_value(Game g, Value value);

void grid_fill(Game g, Value val);
void grid_rand(Game g, size_t low, size_t high);

void grid_render_lines(Game g);
void grid_render_values(Game g);
void game_render(Game g);

Pos coords_to_pos(Vector2 vpos);
Value char_to_value(char keyPress);

#define HAS_SELECTED_CELL(g) (((g).select.x != -1) && ((g).select.y != -1))

// ---------------
// Logic

Game game_init(size_t cols, size_t rows) {
  Game g = { 0 };
  g.cols = cols;
  g.rows = rows;
  g.grid = calloc(cols * rows, sizeof(Value));
  g.select = (Pos){-1, -1};
  return g;
}

void game_close(Game g) { free(g.grid); }

Game game_set_selection(Game g, Pos pos) {
  // TODO: check if the position is legal. That is, we cannot simply
  // change any square. It has to be a sqare on which we have control
  // over.
  g.select = pos;
  return g;
}

void game_set_value(Game g, Value value) {
  if (!HAS_SELECTED_CELL(g) || value == V_INVALID) { return g; }
  Pos select = g.select;
  g.grid[select.y * g.cols + select.x] = value;
}

void grid_fill(Game g, Value val) {
  for (size_t y = 0; y < g.rows; y++) {
    for (size_t x = 0; x < g.cols; x++) {
      g.grid[y * g.cols + x] = V_NONE;
    }
  }
}

void grid_rand(Game g, size_t low, size_t high) {
  for (size_t y = 0; y < g.rows; y++) {
    for (size_t x = 0; x < g.cols; x++) {
      g.grid[y * g.cols + x] = low + (rand() % (high - low));
    }
  }
}

Pos coords_to_pos(Vector2 vpos) {
  Pos pos = { 0 };
  pos.x = floorf(vpos.x / CELL_WIDTH);
  pos.y = floorf(vpos.y / CELL_HEIGHT);
  return pos;
}

Value char_to_value(char keyPress) {
  if (keyPress < '0' || keyPress > '9') {
    return V_INVALID;
  } else {
    return (Value)(keyPress - '0');
  }
}

// ---------------
// Rendering

void grid_render_lines(Game g) {

  size_t thick_bold = 5;
  size_t thick_light = 1;

  // horizontal lines (moving through rows)
  for (size_t y = 1; y < g.rows; y++) {
    Vector2 startPos = { 0, y*CELL_HEIGHT };
    Vector2 endPos = { SCREEN_WIDTH, y*CELL_HEIGHT };
    size_t thickness = y % 3 == 0 ? thick_bold : thick_light;
    DrawLineEx(startPos, endPos, thickness, BLACK);
  }

  // vertical lines (moving through cols)  
  for (size_t x = 1; x < g.cols; x++) {
    Vector2 startPos = { x*CELL_WIDTH, 0 };
    Vector2 endPos = { x*CELL_WIDTH, SCREEN_HEIGHT };
    size_t thickness = x % 3 == 0 ? thick_bold : thick_light;    
    DrawLineEx(startPos, endPos, thickness, BLACK);
  }
}

void grid_render_values(Game g) {
  for (size_t y = 0; y < g.rows; y++) {
    for (size_t x = 0; x < g.cols; x++) {
      Value val = g.grid[y * g.cols + x];
      if (val != V_NONE) {
	char text[80];
	sprintf(text, "%u", val);
	size_t posX = floorf(x * CELL_WIDTH + CELL_WIDTH / 3);
	size_t posY = floorf(y * CELL_HEIGHT + CELL_HEIGHT / 3);
	DrawText(text, posX, posY, 50, BLACK);
      }
    }
  }
}

void game_render(Game g) {
  if (HAS_SELECTED_CELL(g)) {
    size_t posX = g.select.x * CELL_WIDTH;
    size_t posY = g.select.y * CELL_HEIGHT;
    DrawRectangle(posX, posY, CELL_WIDTH, CELL_HEIGHT, LIGHTGRAY);      
  }
  
  grid_render_lines(g);
  grid_render_values(g);
}

//------------------------------------------------------------------------------------
// Program main entry posize_t
//------------------------------------------------------------------------------------
int main(void)
{
  // Initialization
  //--------------------------------------------------------------------------------------
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "sudoku");
  srand(time(NULL));

  Game g = game_init(BOARD_COLS, BOARD_ROWS);
  // grid_rand(g, 0, 9);
  
  SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())    // Detect window close button or ESC key
    {
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
	Vector2 mousePos = GetMousePosition();
	Pos pos = coords_to_pos(mousePos);
	g = game_set_selection(g, pos);	
      }

      char keyPress = GetCharPressed();
      Value val = char_to_value(keyPress);
      if (val != V_INVALID) {
	game_set_value(g, val);
      }

      // Draw
      //----------------------------------------------------------------------------------
      BeginDrawing();
      ClearBackground(RAYWHITE);
      
      game_render(g);
      
      EndDrawing();
      //----------------------------------------------------------------------------------
    }

  // De-Initialization
  game_close(g);  
  //--------------------------------------------------------------------------------------
  CloseWindow();        // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
