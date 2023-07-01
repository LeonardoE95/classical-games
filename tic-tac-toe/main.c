// Author: Leonardo Tamiano
// Made for youtube video: https://youtu.be/1FdujwZ1r8A

#include <stdio.h>
#include <ncurses.h>
#include <assert.h>
#include <stdbool.h>

#define BOARD_WIDTH 3
#define BOARD_HEIGHT 3

#define START_Y_BOARD 0
#define START_X_BOARD 0

#define PLAYER_O 0
#define PLAYER_X 1

#define PLAYER_ICON(x) ((x) == PLAYER_O) ? "O" : "X"

int BOARD[BOARD_HEIGHT][BOARD_WIDTH];

// ---------------------------------------

// Game logic functions

void check_pos(int y, int x) {
  assert(x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT && "invalid position");
}

void board_init(int board[BOARD_HEIGHT][BOARD_WIDTH]) {
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      BOARD[y][x] = -1;
    } 
  }
}

char *board_get_value(int board[BOARD_HEIGHT][BOARD_WIDTH], int y, int x) {
  check_pos(y, x);
  return board[y][x] == -1 ? " " : PLAYER_ICON(board[y][x]);
}

bool board_update(int board[BOARD_HEIGHT][BOARD_WIDTH], int y, int x, int player) {
  check_pos(y, x);

  bool valid_update = true;
  if (board[y][x] != -1) {
    valid_update = false;
  } else {
    board[y][x] = player;
  }
  return valid_update;
}

bool board_check_victory(int board[BOARD_HEIGHT][BOARD_WIDTH], int last_y, int last_x) {
  int val = board[last_y][last_x];

  int y = 0;
  int x = 0;

  // check column
  for (y = 0; (y < BOARD_HEIGHT) && (board[y][last_x] == val); y++) {}
  if (y == BOARD_HEIGHT) { return true; }

  // check row
  for (x = 0; (x < BOARD_WIDTH) && (board[last_y][x] == val); x++) {}
  if (x == BOARD_WIDTH) { return true; }  

  // check diagonals
  if (BOARD_WIDTH == BOARD_HEIGHT) {
    for(x = 0, y = 0; (x < BOARD_WIDTH) && (y < BOARD_HEIGHT) && (board[y][x] == val); x++, y++) {};
    if ((x == BOARD_WIDTH) && (y == BOARD_HEIGHT)) { return true; }

    for(x = BOARD_WIDTH - 1, y = 0; (x > -1) && (y < BOARD_HEIGHT) && (board[y][x] == val); x--, y++) {};
    if ((x == -1) && (y == BOARD_HEIGHT)) { return true; }
  }
  
  return false;
}

// ---------------------------------------

// Game rendering functions

int allow_event(int event_y, int event_x, int start_y, int start_x) {

  // x is out of bounds
  if (event_x < start_x + 2 || event_x > start_x + 2 + 2 * BOARD_WIDTH) {
    return false;
  }

  // y is out of bounds
  if (event_y < start_y + 2 || event_y > start_y + 2 + BOARD_HEIGHT - 1) {
    return false;
  }  

  // (y,x) is on board internal boundaries
  if (((event_x - start_x) % 2) == 0) {
    return false;
  }
  
  return true;
}

int shift_coord(int coord, int start_coord, bool horiz_mode) {
  return horiz_mode ? (coord - start_coord) / 2 : coord - start_coord;
}

void board_write_to_screen(int board[BOARD_HEIGHT][BOARD_WIDTH], int start_y, int start_x) {
  mvprintw(start_y, start_x, "Board Status: ");

  int col = start_y + 2;
  int row = start_x + 2;
  for (int y = 0; y < BOARD_HEIGHT; y++, col++) {
    move(col, row);  
    for (int x = 0; x < BOARD_WIDTH; x++) {
      char *board_value = board_get_value(board, y, x);
      char *separator = x == BOARD_WIDTH - 1 ? "|" : "\0";
      printw("|%s%s", board_value, separator);
    }
  }
}

void player_write_to_screen(int player, int start_y, int start_x, int board_height, int board_width) {
  move(start_y + board_height + 3, start_x + 2);
  printw("Current player: %s\n", PLAYER_ICON(player));
}

// ---------------------------------------

int main() {
  // init ncurses stuff
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  mousemask(ALL_MOUSE_EVENTS, NULL);

  // init my stuff
  board_init(BOARD);

  // main game loop
  int current_player = PLAYER_X;
  int count = 0;
  bool quit = false;
  bool victory = false;
  bool tie = false;

  while(!quit) {

    // render stuff
    board_write_to_screen(BOARD, START_Y_BOARD, START_X_BOARD);
    player_write_to_screen(current_player, START_Y_BOARD, START_X_BOARD, BOARD_HEIGHT, BOARD_WIDTH);
    refresh();

    // handle mouse events
    MEVENT event;
    int ch = getch();
    if (ch == KEY_MOUSE) {
      if(getmouse(&event) == OK) {
	int x = shift_coord(event.x, START_X_BOARD + 2, true);
	int y = shift_coord(event.y, START_Y_BOARD + 2, false);
	
	if (!allow_event(event.y, event.x, START_Y_BOARD, START_X_BOARD)) {
	  // mvprintw(15, 15, "Invalid event!");
	  // mvprintw(16, 16, "(%d, %d) -> (%d, %d)", event.y, event.x, y, x);
	  continue;
	}
	
	// debug info
	// mvprintw(15, 15, "Valid event!");
	// mvprintw(16, 16, "(%d, %d) -> (%d, %d)", event.y, event.x, y, x);

	bool valid_move = board_update(BOARD, y, x, current_player);
	if(!valid_move) { continue; }

	victory = board_check_victory(BOARD, y, x);
	tie = !victory && count == BOARD_WIDTH * BOARD_HEIGHT - 1;

	if (victory) {
	  mvprintw(START_Y_BOARD + BOARD_HEIGHT + 2 + 5, START_X_BOARD + 4, "Player: %s has won!\n", PLAYER_ICON(current_player));
	} else if (tie) {
	  mvprintw(START_Y_BOARD + BOARD_HEIGHT + 2 + 5, START_X_BOARD + 4, "Game is tied, gg wp!\n");
	}	
	
	// update for next iteration
	quit = victory | tie;	
	current_player = current_player == PLAYER_X ? PLAYER_O : PLAYER_X;
	count += 1;
      }
    }
  }

  board_write_to_screen(BOARD, START_Y_BOARD, START_X_BOARD);
  refresh();
  getch();
  
  endwin();
  return 0;
}
