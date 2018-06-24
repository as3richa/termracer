#ifndef _GAMESTATE_T_
#define _GAMESTATE_T_

#include <time.h>

enum
{
  GAMESTATE_CHAR_NORMAL,
  GAMESTATE_CHAR_CORRECT,
  GAMESTATE_CHAR_ERRONEOUS,
};

typedef struct
{
  int started: 1;
  int won: 1;

  time_t started_at;

  int n_keystrokes;
  int n_correct_keystrokes;

  char* text;
  int length;

  char* input;
  int max_input_length;

  int cursor_index;
  int erroneous_index;
} gamestate_t;

int gamestate_init(gamestate_t* gs, const char* text, int length);
void gamestate_destroy(gamestate_t* gs);

void gamestate_press(gamestate_t* gs, char ch);
void gamestate_backspace(gamestate_t* gs);

int gamestate_text_char_at_relative(gamestate_t* gs, int relative_index, int* type);
int gamestate_input_char_at_relative(gamestate_t* gs, int relative_index, int* type);
int gamestate_wpm(gamestate_t* gs);
double gamestate_accuracy(gamestate_t* gs);

#define gamestate_started(gs) ((gs)->started)
#define gamestate_won(gs) ((gs)->won)
#define gamestate_erroneous(gs) ((gs)->erroneous_index != -1)

#endif
