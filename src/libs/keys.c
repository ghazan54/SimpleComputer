#include <sc/CU.h>
#include <sc/helper.h>
#include <sc/interface.h>
#include <sc/keys.h>
#include <sc/ram-operations.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

static struct termios term_settings;

int
rk_readkey (enum keys *key)
{
  char bf[6] = { 0 };
  if (rk_mytermsave ())
    return ERROR_CODE;
  rk_mytermregime (0, 0, 1, 0, 1);
  if (read (STDIN_FILENO, bf, 5) == -1)
    {
      rk_mytermrestore ();
      return ERROR_CODE;
    }
  if (rk_mytermrestore ())
    return ERROR_CODE;
  if (!strcmp ("l", bf))
    {
      *key = key_L;
    }
  else if (!strcmp ("s", bf))
    {
      *key = key_S;
    }
  else if (!strcmp ("r", bf))
    {
      *key = key_R;
    }
  else if (!strcmp ("t", bf))
    {
      *key = key_T;
    }
  else if (!strcmp ("i", bf))
    {
      *key = key_I;
    }
  else if (!strcmp (KEY_F5, bf))
    {
      *key = key_F5;
    }
  else if (!strcmp (KEY_F6, bf))
    {
      *key = key_F6;
    }
  else if (!strcmp (KEY_UP, bf))
    {
      *key = key_UP;
    }
  else if (!strcmp (KEY_DOWN, bf))
    {
      *key = key_DOWN;
    }
  else if (!strcmp (KEY_RIGHT, bf))
    {
      *key = key_RIGHT;
    }
  else if (!strcmp (KEY_LEFT, bf))
    {
      *key = key_LEFT;
    }
  else if (!strcmp ("\n", bf))
    {
      *key = key_ENTER;
    }
  else
    {
      return ERROR_CODE;
    }
  return SUCCES_CODE;
}

int
rk_keyaction (const enum keys key)
{
  char bf[32] = { 0 };
  switch (key)
    {
    case key_L:
      I_printInputField (1, "path: ");
      scanf ("%s", bf);
      I_printInputField (0, NULL);
      return sc_memoryLoad (bf) || I_printall ();
    case key_S:
      mkdir ("data", S_IRWXU | S_IRWXG | S_IRWXO);
      return sc_memorySave (SAVE_PATH);
    case key_R:
      startcu = 2;
      halt_ex = false;
      return SUCCES_CODE;
    case key_T:
      startcu = 1;
      return SUCCES_CODE;
    case key_I:
      return I_restartsc ();
    case key_F5:
      return I_setAccumulator ();
    case key_F6:
      return I_setInstructionCounter ();
    case key_UP:
      return I_move_address_xy (0);
    case key_DOWN:
      return I_move_address_xy (1);
    case key_RIGHT:
      return I_move_address_xy (2);
    case key_LEFT:
      return I_move_address_xy (3);
    case key_ENTER:
      return cu_read (instructionCounter)
             || I_printhex (instructionCounter, color_red, color_default);
    default:
      // I_printOutputField("Unknow key");
      break;
    }
  return ERROR_CODE;
}

int
rk_mytermsave (void)
{
  return tcgetattr (STDIN_FILENO, &term_settings) != -1 ? SUCCES_CODE
                                                        : ERROR_CODE;
}

int
rk_mytermrestore (void)
{
  return tcsetattr (STDIN_FILENO, TCSANOW, &term_settings) != -1 ? SUCCES_CODE
                                                                 : ERROR_CODE;
}

int
rk_mytermregime (int regime, int vtime, int vmin, int echo, int sigint)
{
  struct termios t;
  if (tcgetattr (STDIN_FILENO, &t) == -1)
    return ERROR_CODE;
  if (regime)
    {
      t.c_lflag &= ICANON | ECHO;
    }
  else
    {
      t.c_lflag &= ~ICANON;
      t.c_lflag = echo ? t.c_lflag | ECHO : t.c_lflag & (~ECHO);
      t.c_lflag = sigint ? t.c_lflag | ISIG : t.c_lflag & (~ISIG);
      t.c_cc[VMIN] = vmin;
      t.c_cc[VTIME] = vtime;
    }
  return tcsetattr (STDIN_FILENO, TCSANOW, &t) != -1 ? SUCCES_CODE
                                                     : ERROR_CODE;
}
