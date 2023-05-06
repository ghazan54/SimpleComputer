#include <errno.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <net/if.h>
#include <sc/helper.h>
#include <sc/terminal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int
mt_clrscr (void)
{
  return write (STDOUT_FILENO, "\e[H\e[J", strlen ("\e[H\e[J")) != -1
             ? SUCCES_CODE
             : ERROR_CODE;
}

int
mt_getscreensize (int *rows, int *columns)
{
  struct winsize ws;
  if (!rows || !columns || ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws))
    {
      return ERROR_CODE;
    }
  else
    {
      *rows = ws.ws_row;
      *columns = ws.ws_col;
      return SUCCES_CODE;
    }
}

int
mt_gotoXY (int x, int y)
{
  int row, col;
  if (mt_getscreensize (&row, &col) || ((x > row) || (x < 0))
      || ((y > col) || (y < 0)))
    {
      return ERROR_CODE;
    }
  char s[BUFSIZ];
  sprintf (s, "\e[%d;%dH", x, y);
  return write (STDOUT_FILENO, s, strlen (s)) != -1 ? SUCCES_CODE : ERROR_CODE;
}

int
mt_setfgcolor (enum colors color)
{
  char s[BUFSIZ];
  sprintf (s, "\e[0;3%dm", color);
  return write (STDOUT_FILENO, s, strlen (s)) != -1 ? SUCCES_CODE : ERROR_CODE;
}

int
mt_setbgcolor (enum colors color)
{
  char s[BUFSIZ];
  sprintf (s, "\e[4%dm", color);
  return write (STDOUT_FILENO, s, strlen (s)) != -1 ? SUCCES_CODE : ERROR_CODE;
}
