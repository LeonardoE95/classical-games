#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#define SCREEN_WIDTH  900
#define SCREEN_HEIGHT 900

#define BOARD_COLS 9
#define BOARD_ROWS 9

#define CELL_WIDTH ((SCREEN_WIDTH / BOARD_COLS))
#define CELL_HEIGHT ((SCREEN_HEIGHT / BOARD_ROWS))

#define FIXED_VALUE_COLOR BLACK
#define DYNAMIC_VALUE_COLOR BLACK

//----------------------------------------------------------------------------------

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
} CellValue;

typedef enum {
  FIXED = 0, 
  DYNAMIC
} CellType;

typedef struct {
  CellValue value;
  CellType type;
} Cell;

typedef struct {
  size_t x;
  size_t y;
} Pos;

typedef enum {
  E_NONE = 0,
  E_ROW,
  E_COL,
  E_SQUARE,
} ErrorType;

typedef struct {
  ErrorType type;  
  size_t index;  
} Error;

typedef struct {
  size_t cols;
  size_t rows;
  Cell *grid;
  
  Pos select;
  Error error;
  uint8_t victory;
} Game;

//----------------------------------------------------------------------------------

Game game_init_from_file(const char *path);
Game game_init(size_t cols, size_t rows);
void game_close(Game g);
Game game_check_and_set_selection(Game g, Pos pos);
Game game_check_and_set_value(Game g, CellValue value, CellType type);
Game game_check_and_set_error(Game g);
Game game_check_and_set_victory(Game g);

void grid_fill(Game g, CellValue val);
void grid_rand(Game g, size_t low, size_t high);

void grid_render_lines(Game g);
void grid_render_values(Game g);
void game_render(Game g);

Pos coords_to_pos(Vector2 vpos);
CellValue char_to_value(char keyPress);

#define HAS_SELECTED_CELL(g) (((g).select.x != -1) && ((g).select.y != -1))
#define HAS_VALUE(c) (((c).value != V_INVALID) && ((c).value != V_NONE))
#define CELL_IS_DYNAMIC(g, pos) ((g).grid[(pos).y * (g).cols + (pos).x].type == DYNAMIC)
#define SELECTED_CELL_IS_DYNAMIC(g) CELL_IS_DYNAMIC((g), (g).select)

//----------------------------------------------------------------------------------

Game game_init(size_t cols, size_t rows) {
  Game g = { 0 };
  g.cols = cols;
  g.rows = rows;
  g.grid = calloc(cols * rows, sizeof(Cell));
  g.select = (Pos){-1, -1};
  g.error = (Error){ E_NONE, 0 };  
  return g;
}

void game_close(Game g) { free(g.grid); }

Game game_check_and_set_selection(Game g, Pos pos) {
  // only select to dynamic cells
  if (CELL_IS_DYNAMIC(g, pos)) {
    g.select = pos;    
  }

  return g;
}

Game game_check_and_set_value(Game g, CellValue value, CellType type) {
  if (HAS_SELECTED_CELL(g) && SELECTED_CELL_IS_DYNAMIC(g) && (value != V_INVALID)) {
    g.grid[g.select.y * g.cols + g.select.x] = (Cell){ value, type };
  }
  return g;
}

Game game_check_and_set_error(Game g) {
  g.error = (Error) { E_NONE, 0 };
 
  // each row, colum and square must contain 9 distinct digits, from 1 through 9.
  
  // check rows
  for(int y = 0; y < g.rows; y++) {
    size_t seen[10] = { 0 };
    for(int x = 0; x < g.cols; x++) {
      Cell c = g.grid[y * g.cols + x];
      if (HAS_VALUE(c)) {
	seen[c.value] += 1;
	if (seen[c.value] > 1) {
	  g.error = (Error) { E_ROW, y };
	}
      }
    }
  }

  // check cols
  for(int x = 0; x < g.cols; x++) {
    size_t seen[10] = { 0 };
    for(int y = 0; y < g.rows; y++) {
      Cell c = g.grid[y * g.cols + x];
      if (HAS_VALUE(c)) {
	seen[c.value] += 1;
	if (seen[c.value] > 1) {
	  g.error = (Error) { E_COL, x };
	}
      }
    }
  }

  // TODO: check squares only if we have a square grid
  // NOTE: for now we have hard-coded a value of 3
  if (BOARD_COLS == BOARD_ROWS) {
    for (int i = 0; i < 9; i++) {
      size_t seen[10] = { 0 };

      size_t start_y = floorf(i / 3) * 3;
      size_t start_x = (i % 3) * 3;                

      // printf("(%d, %d)\n", start_x, start_y);
      for (size_t y = start_y; y < start_y + 3; y++) {
	for (size_t x = start_x; x < start_x + 3; x++) {
	  Cell c = g.grid[y * g.cols + x];
	  if (HAS_VALUE(c)) {
	    seen[c.value] += 1;
	    if (seen[c.value] > 1) {
	      g.error = (Error) { E_SQUARE, i };
	    }
	  }
	}
      }      
    }    
  }
    
  return g;
};
  
Game game_check_and_set_victory(Game g) {
  // TODO: check victory state. Simply count that all cells are non
  // empty and that there are no errors. This should be enough.
  return g;
};

void grid_fill(Game g, CellValue val) {
  for (size_t y = 0; y < g.rows; y++) {
    for (size_t x = 0; x < g.cols; x++) {
      g.grid[y * g.cols + x] = (Cell){ V_NONE, DYNAMIC };
    }
  }
}

// TODO: implement this properly
// void grid_rand(Game g, size_t low, size_t high) {
//   for (size_t y = 0; y < g.rows; y++) {
//     for (size_t x = 0; x < g.cols; x++) {
//       g.grid[y * g.cols + x] = low + (rand() % (high - low));      
//     }
//   }
// }

Game game_init_from_file(const char *path) {
  FILE *f = fopen(path,"r");

  if (f == NULL) {
    fprintf(stderr, "ERROR: could not open file %s: %s\n", path, strerror(errno));
    exit(1);
  }

  char *line;
  size_t len = 0;
  size_t read;
  
  read = getline(&line, &len, f);

  if (read == -1) {
    fprintf(stderr, "ERROR: could not read line %d from %s: %s\n", 0, path, strerror(errno));
    exit(1);
  }
    
  char *rows_str = strtok(line, ",");
  char *cols_str = strtok(NULL, ",");
  char *null_str = strtok(NULL, ",");

  if (!rows_str || !cols_str || null_str) {
    fprintf(stderr, "ERROR: grid file not properly formatted %s\n", path);
    exit(1);      
  }

  // NOTE: should more checks be needed here?
  int rows, cols;  
  rows = atoi(rows_str);
  cols = atoi(cols_str);

  if (!rows) {
    fprintf(stderr, "ERROR: could not atoi rows value (%s)\n", rows_str);
    exit(1);      
  }

  if (!cols) {
    fprintf(stderr, "ERROR: could not atoi cols value (%s)\n", cols_str);
    exit(1);      
  }    

  Game g = game_init(cols, rows);
  
  // NOTE: probably more checks needed here
  for (size_t y = 0; y < g.rows; y++) {
    read = getline(&line, &len, f);

    if (read == -1) {
      fprintf(stderr, "ERROR: could not read line %d from %s: %s\n", y+1, path, strerror(errno));
      exit(1);
    }

    for (size_t x = 0; x < g.cols; x++) {
      char *value_str = strtok(x == 0 ? line : NULL, ",");
      CellValue val;
      if (*value_str == 'X') {
	g.grid[y * g.cols + x] = (Cell){ V_NONE, DYNAMIC };
      } else if ((val = char_to_value(*value_str)) != V_INVALID) {
	g.grid[y * g.cols + x] = (Cell){ val, FIXED };
      } else {
	fprintf(stderr, "ERROR: Invalid value found (%c) when reading file %s \n", *value_str, path);
	exit(1);
      }
    }
  }
  
  fclose(f);

  return g;
}

//----------------------------------------------------------------------------------

Pos coords_to_pos(Vector2 vpos) {
  Pos pos = { 0 };
  pos.x = floorf(vpos.x / CELL_WIDTH);
  pos.y = floorf(vpos.y / CELL_HEIGHT);
  return pos;
}

CellValue char_to_value(char keyPress) {
  if (keyPress < '0' || keyPress > '9') {
    return V_INVALID;
  } else {
    return (CellValue)(keyPress - '0');
  }
}

//----------------------------------------------------------------------------------

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
      CellValue val = g.grid[y * g.cols + x].value;
      CellType type = g.grid[y * g.cols + x].type;
      if (val != V_NONE) {
	char text[80];
	sprintf(text, "%u", val);
	size_t posX = floorf(x * CELL_WIDTH + CELL_WIDTH / 3);
	size_t posY = floorf(y * CELL_HEIGHT + CELL_HEIGHT / 3);

	DrawText(text, posX, posY, 50, type == FIXED ? FIXED_VALUE_COLOR : DYNAMIC_VALUE_COLOR);
      }
    }
  }
}

void grid_render_error(Game g) {
  size_t posX, posY;  
  switch(g.error.type)  {
  case E_NONE:
    break;
    
  case E_ROW:
    posX = 0;
    posY = g.error.index * CELL_HEIGHT;
    DrawRectangle(posX, posY, BOARD_COLS*CELL_WIDTH, CELL_HEIGHT, RED);
    break;

  case E_COL:
    posX = g.error.index * CELL_WIDTH;
    posY = 0;
    DrawRectangle(posX, posY, CELL_WIDTH, BOARD_ROWS*CELL_HEIGHT, RED);
    break;

  case E_SQUARE:
    posX = (g.error.index % 3) * 3 * CELL_WIDTH;
    posY = floorf(g.error.index / 3) * 3 * CELL_HEIGHT;    
    DrawRectangle(posX, posY, CELL_WIDTH*3, CELL_HEIGHT*3, RED);
    break;

  default:
    assert("Unreachable\n");
    break;
  }
  
}

void game_render(Game g) {
  if (HAS_SELECTED_CELL(g)) {
    size_t posX = g.select.x * CELL_WIDTH;
    size_t posY = g.select.y * CELL_HEIGHT;
    DrawRectangle(posX, posY, CELL_WIDTH, CELL_HEIGHT, LIGHTGRAY);      
  }

  grid_render_error(g);      
  grid_render_lines(g);
  grid_render_values(g);
}

//----------------------------------------------------------------------------------

int main(void)
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "sudoku");
  SetTargetFPS(60);
  
  // Game g = game_init(BOARD_COLS, BOARD_ROWS);
  // srand(time(NULL));  
  // grid_rand(g, 0, 9);
  
  Game g = game_init_from_file("./data/example2.txt");
  
  while (!WindowShouldClose())
    {
      // Manage state
      //----------------------------------------------------------------------------------

      // Select a cell
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
	Vector2 mousePos = GetMousePosition();
	Pos pos = coords_to_pos(mousePos);
	// printf("(%d, %d)\n", pos.x, pos.y);
	g = game_check_and_set_selection(g, pos);
      }

      // Write a number
      char keyPress = GetCharPressed();
      CellValue val = char_to_value(keyPress);
      game_check_and_set_value(g, val, DYNAMIC);

      // update internal state
      g = game_check_and_set_error(g);
      g = game_check_and_set_victory(g);
      
      // Render
      //----------------------------------------------------------------------------------      
      BeginDrawing();
      ClearBackground(RAYWHITE);
      game_render(g);
      EndDrawing();
    }

  game_close(g);  
  CloseWindow();

  return 0;
}
