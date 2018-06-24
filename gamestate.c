#include "gamestate.h"

#include <stdlib.h> 
#include <stdio.h>  
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

int gamestate_init(gamestate_t* gs, const char* text, int length)
{
  gs->started = 0;
  gs->won = 0;

  gs->n_keystrokes = 0;
  gs->n_correct_keystrokes = 0;

  gs->length = length;
  gs->max_input_length = length + 200;

  gs->text = (char*)malloc(length);

  if(!gs->text)
  {
    return -1;
  }

  memcpy(gs->text, text, length);

  gs->input = (char*)malloc(gs->max_input_length);

  if(!gs->input)
  {
    free(gs->text);
    return -1;
  }

  gs->cursor_index = 0;
  gs->erroneous_index = -1;

  return 0;
}

void gamestate_destroy(gamestate_t* gs)
{
  free(gs->text);
  free(gs->input);
}

void gamestate_press(gamestate_t* gs, char ch)
{
  if(!isprint(ch))
  {
    return;
  }

  if(gs->cursor_index + 1 >= gs->max_input_length)
  {
    return;
  }

  if(gs->won)
  {
    return;
  }

  if(!gs->started)
  {
    gs->started = 1;
    gs->started_at = time(NULL);
  }

  gs->input[gs->cursor_index] = ch;
  gs->n_keystrokes ++;

  if(gs->erroneous_index == -1)
  {
    if(gs->cursor_index >= gs->length || ch != gs->text[gs->cursor_index])
    {
      gs->erroneous_index = gs->cursor_index;
    }
    else
    {
      gs->n_correct_keystrokes ++;
      gs->won = gs->cursor_index == gs->length - 1;
    } 
  }

  gs->cursor_index ++;
}

void gamestate_backspace(gamestate_t* gs)
{
  if(gs->won || !gs->started)
  {
    return;
  }

  if(gs->cursor_index == 0)
  {
    return;
  }

  gs->cursor_index --;

  if(gs->cursor_index <= gs->erroneous_index)
  {
    gs->erroneous_index = -1;
  }
}

int gamestate_text_char_at_relative(gamestate_t* gs, int relative_index, int* type)
{
  const int real_index = relative_index + gs->cursor_index;

  if(real_index < 0 || real_index >= gs->length)
  {
    return EOF;
  }
  
  if(gs->cursor_index <= real_index)
  {
    *type = GAMESTATE_CHAR_NORMAL;
  }
  else if(gs->erroneous_index != -1 && gs->erroneous_index <= real_index)
  {
    *type = GAMESTATE_CHAR_ERRONEOUS;
  }
  else
  {
    *type = GAMESTATE_CHAR_CORRECT;
  }

  return gs->text[real_index];
}

int gamestate_input_char_at_relative(gamestate_t* gs, int relative_index, int* type)
{
  const int real_index = relative_index + gs->cursor_index;

  if(real_index < 0 || real_index >= gs->cursor_index)
  {
    return EOF;
  }

  if(gs->erroneous_index != -1 && gs->erroneous_index <= real_index)
  {
    *type = GAMESTATE_CHAR_ERRONEOUS;
  }
  else
  {
    *type = GAMESTATE_CHAR_CORRECT;
  }

  return gs->input[real_index];
}

int gamestate_wpm(gamestate_t* gs)
{
  if(!gs->started)
  {
    return 0;
  }

  double elapsed_seconds = time(NULL) - gs->started_at;

  if(elapsed_seconds < 1)
  {
    elapsed_seconds = 1;
  }

  const double words = gs->n_correct_keystrokes / 5.0;

  return (int)roundl(words / elapsed_seconds * 60);
}

double gamestate_accuracy(gamestate_t* gs)
{
  return (gs->n_keystrokes == 0) ? 0 : ((double)gs->n_correct_keystrokes / gs->n_keystrokes);
}
