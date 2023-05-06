#pragma once

#define TERMINAL "/dev/tty"

#define TERM_CLEAR_SCREEN ("\e[H\e[J")
#define TERM_GOTO_MIN_SIZE (5) // \e[;H
#define TERM_GOTO_X "\e["
#define TERM_GOTO_Y "H"
#define TERM_FGCOLOR_MIN_SIZE (5) // \e[3m
#define TERM_FGCOLOR_FP "\e[3"
#define TERM_FGCOLOR_SP "m"
#define TERM_BGCOLOR_MIN_SIZE (5) // \e[4m
#define TERM_BGCOLOR_FP "\e[4"
#define TERM_BGCOLOR_SP "m"

enum colors
{
  color_black = 0,
  color_red,
  color_green,
  color_yellow,
  color_blue,
  color_purple,
  color_aqua,
  color_white,
  color_default = 9
};

int mt_clrscr (void);
int mt_gotoXY (int x, int y);
int mt_getscreensize (int *rows, int *cols);
int mt_setfgcolor (enum colors);
int mt_setbgcolor (enum colors);