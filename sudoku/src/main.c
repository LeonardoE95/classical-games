#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

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
} Value;

typedef struct {
  int x;
  int y;  
} Pos;

typedef struct {
  int cols;
  int rows;
  Value *grid;
  Pos select;
} Game;

// ---------------
// headers

Game game_init(int cols, int rows);
void game_close(Game g);
Game game_set_selection(Game g, Pos pos);
void grid_fill(Game g, Value val);
void grid_rand(Game g, int low, int high);
Pos coords_to_pos(Vector2 vpos);

void grid_lines_render(Game g);
void grid_values_render(Game g);
void game_render(Game g);

// ---------------
// Logic

Game game_init(int cols, int rows) {
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

void grid_fill(Game g, Value val) {
  for (int x = 0; x < g.rows; x++) {
    for (int y = 0; y < g.cols; y++) {
      g.grid[x * g.cols + y] = V_NONE;
    }
  }
}

void grid_rand(Game g, int low, int high) {
  for (int x = 0; x < g.rows; x++) {
    for (int y = 0; y < g.cols; y++) {
      g.grid[x * g.cols + y] = low + (rand() % (high - low));
    }
  }  
}

Pos coords_to_pos(Vector2 vpos) {
  Pos pos = { 0 };
  pos.x = floorf(vpos.x / CELL_WIDTH);
  pos.y = floorf(vpos.y / CELL_HEIGHT);
  return pos;
}


// ---------------
// Rendering

void grid_lines_render(Game g) {

  for (int x = 1; x < g.rows; x++) {
    Vector2 startPos = { 0, x*CELL_HEIGHT };
    Vector2 endPos = { SCREEN_WIDTH, x*CELL_HEIGHT };
    int thickness = x % 3 == 0 ? 3 : 1;
    DrawLineEx(startPos, endPos, thickness, BLACK);
  }

  for (int y = 1; y < g.cols; y++) {
    Vector2 startPos = { y*CELL_WIDTH, 0 };
    Vector2 endPos = { y*CELL_WIDTH, SCREEN_HEIGHT };
    int thickness = y % 3 == 0 ? 3 : 1;
    DrawLineEx(startPos, endPos, thickness, BLACK);
  }
}

void grid_values_render(Game g) {
  for (int x = 0; x < g.rows; x++) {
    for (int y = 0; y < g.cols; y++) {
      Value val = g.grid[x * g.cols + y];

      if (val != V_NONE) {
	char text[80];
	sprintf(text, "%d", val);

	int posX = floorf(y * CELL_WIDTH + CELL_WIDTH / 3);
	int posY = floorf(x * CELL_HEIGHT + CELL_HEIGHT / 5);
      
	DrawText(text, posX, posY, 50, BLACK);
      }      
    }
  }  
}

void game_render(Game g) {
  if ((g.select.x != -1) && (g.select.y != -1)) {
    int posX = g.select.x * CELL_WIDTH;
    int posY = g.select.y * CELL_HEIGHT;
    DrawRectangle(posX, posY, CELL_WIDTH, CELL_HEIGHT, LIGHTGRAY);
  }
  
  grid_lines_render(g);
  
  grid_values_render(g);
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
  // Initialization
  //--------------------------------------------------------------------------------------
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "sudoku");
  srand(time(NULL));

  Game g = game_init(BOARD_COLS, BOARD_ROWS);
  // grid_rand(g, 0, 9);  
    
  // for(int x = 0; x < g.rows; x++) {
  //   for(int y = 0; y < g.cols; y++) {
  //     printf("%d ", g.grid[x * g.cols + y]);
  //   }
  //   printf("\n");
  // }

  // return;
  
  SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())    // Detect window close button or ESC key
    {
      // Update
      //----------------------------------------------------------------------------------
      // TODO: Update your variables here
      //----------------------------------------------------------------------------------
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
	Vector2 mousePos = GetMousePosition();
	Pos pos = coords_to_pos(mousePos);
	g = game_set_selection(g, pos);	
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
