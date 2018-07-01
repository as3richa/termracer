#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

#include "gamestate.h"
#include "corpus.h"

const int CURSOR_LEFT_OFFSET = 15;
const int MIN_HEIGHT = 6;
const int MIN_WIDTH = 50;

const char* default_text = "Fly me to the moon and let me play among the stars. Let me see what spring is like on Jupiter and Mars. In other words, hold my hand.";

enum
{
  PAIR_RE_BL = 1,
  PAIR_GR_BL
};

int ncurses_active;
void setup_ncurses(void);
void teardown_ncurses(void);

void render(gamestate_t*);
int type2attr(int, int);

int main(void)
{
  srand(time(NULL));

  const char* candidate_corpus_filenames[3] = {
    getenv("TERMRACER_CORPUS"),
    "/etc/termracer/corpus.list",
    "./corpus.list"
  };

  char* text = NULL;
  int length;

  for(int i = 0; i < 3; i ++)
  {
    text = corpus_select_text(candidate_corpus_filenames[i], &length);

    if(text != NULL)
    {
      break;
    }
  }

  if(text == NULL)
  {
    length = strlen(default_text);
    text = (char*)malloc(length);

    if(text == NULL)
    {
      return 1;
    }

    memcpy(text, default_text, length);
  }

  setup_ncurses();
  atexit(teardown_ncurses);

  gamestate_t gs;

  if(gamestate_init(&gs, text, length) == -1)
  {
    return 1;
  }

  free(text);

  render(&gs);

  while(!gs.won)
  {
    int ch = getch();

    if(ch == KEY_BACKSPACE || ch == KEY_DC || ch == 127)
    {
      gamestate_backspace(&gs);
    }
    else if(0 <= ch && ch < 128)
    {
      gamestate_press(&gs, ch);
    }

    render(&gs);
  }

  teardown_ncurses();
  printf("WPM: %d; ACC: %.1f%%\n", gamestate_wpm(&gs), gamestate_accuracy(&gs) * 100);

  gamestate_destroy(&gs);

  return 0;
}

void setup_ncurses(void)
{
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, 1);
  halfdelay(2);
  clear();

  start_color();
  init_pair(PAIR_RE_BL, COLOR_RED, COLOR_BLACK);
  init_pair(PAIR_GR_BL, COLOR_GREEN, COLOR_BLACK);

  curs_set(0);

  ncurses_active = 1;
}

void teardown_ncurses(void)
{
  if(ncurses_active)
  {
    ncurses_active = 0;
    endwin();
  }
}

void render(gamestate_t* gs)
{
  clear();

  int height, width;
  getmaxyx(stdscr, height, width);

  if(height < MIN_HEIGHT || width < MIN_WIDTH)
  {
    mvprintw(0, 0, "Please enlarge your terminal.");
  }
  else
  {
    attron(A_REVERSE | A_BOLD);
    mvprintw(1, 20, " WPM: %d ", gamestate_wpm(gs));
    mvprintw(1, 32, " ACC: %.1lf%% ", gamestate_accuracy(gs) * 100);
    attroff(A_REVERSE | A_BOLD);

    for(int i = 1; i < width - 1; i ++)
    {
      const int ri = i - CURSOR_LEFT_OFFSET;

      int ch, type;

      ch = gamestate_text_char_at_relative(gs, ri, &type);

      if(ch != EOF)
      {
        int attr = type2attr(type, ri == 0);
        attron(attr);
        mvaddch(3, i, ch);
        attroff(attr);
      }

      if(ri < 0)
      {
        ch = gamestate_input_char_at_relative(gs, ri, &type); 

        if(ch != EOF)
        {
          int attr = type2attr(type, 0);
          attron(attr);
          mvaddch(4, i, ch);
          attroff(attr);
        }
      }
      else if(ri == 0)
      {
        type = gamestate_erroneous(gs) ? GAMESTATE_CHAR_ERRONEOUS : GAMESTATE_CHAR_CORRECT;
        int attr = type2attr(type, 1);

        attron(attr);
        mvaddch(4, i, ' ');
        attroff(attr);
      }
    }
  }

  refresh();
}

int type2attr(int type, int is_cursor)
{
  int attr = is_cursor ? A_REVERSE : 0;

  if(type == GAMESTATE_CHAR_CORRECT)
  {
    attr |= COLOR_PAIR(PAIR_GR_BL);
  }
  else if(type == GAMESTATE_CHAR_ERRONEOUS)
  {
    attr |= COLOR_PAIR(PAIR_RE_BL);
  }

  return attr;
}
