#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <sc/CU.h>
#include <sc/helper.h>
#include <sc/interface.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

static int base_x = 2;
static int base_y = 2;

int instructionCounter = 0;

bool last_jump = false;

int lastsig = -1;

int startcu = false;
int rig = false;

int rignore = 1;
int outpitLine = 0;

bool halt_ex = false;

const int bigChars[][2]
    = { { 0x8181817E, 0x7E818181 }, { 0x40485060, 0x40404040 },
        { 0x2040423C, 0x7E040810 }, { 0x7C40407C, 0x7C404040 },
        { 0x7E424242, 0x40404040 }, { 0x7E02027E, 0x7E404040 },
        { 0x7E02027E, 0x7E424242 }, { 0x4040407E, 0x40404040 },
        { 0x7E42427E, 0x7E424242 }, { 0x7E42427E, 0x7E404040 },
        { 0x7E42423C, 0x42424242 }, { 0x3E42423E, 0x3E424242 },
        { 0x0101017E, 0x7E010101 }, { 0x4242221E, 0x1E224242 },
        { 0x7E02027E, 0x7E020202 }, { 0x7E02027E, 0x02020202 },
        { 0xFF181818, 0x181818FF }, { 0xFF000000, 0xFF } };

int
I_simplecomputer (void)
{
  setvbuf (stdout, NULL, _IONBF, 0);
  if (mt_clrscr () || sc_memoryInit () || sc_regInit ())
    return ERROR_CODE;
  if (mt_gotoXY (1, 1))
    return ERROR_CODE;
  if (bc_box (1, 1, 12, 62) || bc_box (1, 63, 3, 84) || bc_box (4, 63, 6, 84)
      || bc_box (7, 63, 9, 84) || bc_box (10, 63, 12, 84)
      || bc_box (13, 1, 22, 46) || bc_box (13, 47, 22, 84)
      || bc_box (23, 1, 33, 46) || bc_box (23, 47, 33, 84))
    return ERROR_CODE;
  I_printall ();
  I_startsc ();
  return SUCCES_CODE;
}

void
I_stopsc (int sig)
{
  lastsig = sig;
  sc_memoryFree ();
  mt_gotoXY (33, 0);
  exit (SUCCES_CODE);
}

void
I_sigalarm (int sig)
{
  lastsig = sig;
  if (last_jump)
    {
      last_jump = false;
    }
  else
    {
      I_move_address_xy (2);
    }
  I_scstep (0);
}

int
I_startsc ()
{
  startcu = false;
  while (1)
    {
      signal (SIGINT, I_stopsc);
      signal (SIGALRM, I_sigalarm);
      signal (SIGUSR1, I_sigusr1);
      if (rignore)
        {
          I_scstep (rignore);
        }
    }
  return SUCCES_CODE;
}

int
I_scstep (int rignore)
{
  if (I_printinstructionCounter ())
    return ERROR_CODE;
  if (I_printoperations ())
    return ERROR_CODE;
  if (I_printaccumulator ())
    return ERROR_CODE;
  if (I_printflags ())
    return ERROR_CODE;
  if (I_printbig (instructionCounter))
    return ERROR_CODE;
  if (mt_gotoXY (24, 5))
    return ERROR_CODE;
  if (rignore)
    {
      enum keys key = 99;
      rk_readkey (&key);
      rk_keyaction (key);
    }
  if (startcu == 2)
    {
      CU ();
      if (!halt_ex)
        return I_runprogram ();
    }
  else if (startcu == 1)
    {
      CU ();
      return I_move_address_xy (2);
    }
  return SUCCES_CODE;
}

int
I_move_address_xy (const int d)
{
  if (I_printhex (instructionCounter, color_default, color_default))
    return ERROR_CODE;
  switch (d)
    {
    case 0:
      instructionCounter -= 10;
      if (instructionCounter < 0)
        instructionCounter += 10;
      break;
    case 1:
      instructionCounter += 10;
      if (instructionCounter >= DEFAULT_MEMORY_INIT)
        instructionCounter -= 10;
      break;
    case 2:
      instructionCounter += 1;
      if (instructionCounter >= DEFAULT_MEMORY_INIT)
        instructionCounter -= 1;
      break;
    case 3:
      instructionCounter -= 1;
      if (instructionCounter < 0)
        instructionCounter += 1;
      break;

    default:
      break;
    }
  if (I_printhex (instructionCounter, color_red, color_default))
    return ERROR_CODE;
  return SUCCES_CODE;
}

int
I_printall (void)
{
  if (I_printinfo ('a', color_default, color_default))
    return ERROR_CODE;
  if (I_printinfo ('i', color_default, color_default))
    return ERROR_CODE;
  if (I_printinfo ('o', color_default, color_default))
    return ERROR_CODE;
  if (I_printinfo ('f', color_default, color_default))
    return ERROR_CODE;
  if (I_printcustomfields ())
    return ERROR_CODE;
  if (mt_gotoXY (1, 29))
    return ERROR_CODE;
  sc_print (" Memory ");
  for (int i = 0; i < DEFAULT_MAX_STRS; ++i)
    {
      for (int j = 0; j < DEFAULT_MAX_STRS; ++j)
        {
          if (I_printhex (i * DEFAULT_MAX_STRS + j, color_default,
                          color_default))
            return ERROR_CODE;
        }
    }
  if (I_printbig (0))
    return ERROR_CODE;
  if (I_printkeys ())
    return ERROR_CODE;
  if (I_printhex (instructionCounter, color_red, color_default))
    return ERROR_CODE;
  return SUCCES_CODE;
}

int
I_printhex (int ic, enum colors fg, enum colors bg)
{
  int i = ic / 10;
  int j = ic % 10;
  if (mt_setfgcolor (fg))
    return ERROR_CODE;
  if (mt_setbgcolor (bg))
    return ERROR_CODE;
  if (i >= DEFAULT_MAX_STRS || j >= DEFAULT_MAX_STRS || i < 0 || j < 0)
    {
      return ERROR_CODE;
    }
  else
    {
      if (mt_gotoXY (i + base_x, j ? j * 6 + base_y : base_y))
        return ERROR_CODE;
      sc_memoryAddressOutput (i, j);
    }
  if (mt_setfgcolor (color_default))
    return ERROR_CODE;
  if (mt_setbgcolor (color_default))
    return ERROR_CODE;
  return SUCCES_CODE;
}

int
I_printinfo (const char I, enum colors fg, enum colors bg)
{
  int c = 1;
  if (mt_setfgcolor (fg))
    return ERROR_CODE;
  if (mt_setbgcolor (bg))
    return ERROR_CODE;
  switch (I)
    {
    case 'a':
      c = I_printaccumulator ();
      break;
    case 'i':
      c = I_printinstructionCounter ();
      break;
    case 'o':
      c = I_printoperations ();
      break;
    case 'f':
      c = I_printflags ();
      break;
    default:
      c = ERROR_CODE;
      break;
    }
  if (mt_setfgcolor (color_default))
    return ERROR_CODE;
  if (mt_setbgcolor (color_default))
    return ERROR_CODE;
  return c;
}

int
_I_printbig (const char d, int x, int y)
{
  switch (d)
    {
    case '0':
      return bc_printbigchar (bigChars[0], x, y, color_default, color_default);
    case '1':
      return bc_printbigchar (bigChars[1], x, y, color_default, color_default);
    case '2':
      return bc_printbigchar (bigChars[2], x, y, color_default, color_default);
    case '3':
      return bc_printbigchar (bigChars[3], x, y, color_default, color_default);
    case '4':
      return bc_printbigchar (bigChars[4], x, y, color_default, color_default);
    case '5':
      return bc_printbigchar (bigChars[5], x, y, color_default, color_default);
    case '6':
      return bc_printbigchar (bigChars[6], x, y, color_default, color_default);
    case '7':
      return bc_printbigchar (bigChars[7], x, y, color_default, color_default);
    case '8':
      return bc_printbigchar (bigChars[8], x, y, color_default, color_default);
    case '9':
      return bc_printbigchar (bigChars[9], x, y, color_default, color_default);
    case '+':
      return bc_printbigchar (bigChars[16], x, y, color_default,
                              color_default);
    case '-':
      return bc_printbigchar (bigChars[17], x, y, color_default,
                              color_default);
    case 'A':
      return bc_printbigchar (bigChars[10], x, y, color_default,
                              color_default);
    case 'B':
      return bc_printbigchar (bigChars[11], x, y, color_default,
                              color_default);
    case 'C':
      return bc_printbigchar (bigChars[12], x, y, color_default,
                              color_default);
    case 'D':
      return bc_printbigchar (bigChars[13], x, y, color_default,
                              color_default);
    case 'E':
      return bc_printbigchar (bigChars[14], x, y, color_default,
                              color_default);
    case 'F':
      return bc_printbigchar (bigChars[15], x, y, color_default,
                              color_default);
    default:
      return ERROR_CODE;
    }
}

int
I_printbig (int ic)
{
  int c;
  int x = ic / 10;
  int y = ic % 10;
  if (sc_memoryGet (DEFAULT_MAX_STRS * x + y, &c))
    return ERROR_CODE;
  char digit[5];
  int command, operand;
  if (sc_commandDecode (c, &command, &operand))
    return ERROR_CODE;
  sprintf (digit, "%02X%02X", command, operand);
  char sign = c & 0x4000 ? '-' : '+';
  if (_I_printbig (sign, 14, 2) || _I_printbig (digit[0], 14, 11)
      || _I_printbig (digit[1], 14, 20) || _I_printbig (digit[2], 14, 29)
      || _I_printbig (digit[3], 14, 38))
    return ERROR_CODE;
  return SUCCES_CODE;
}

int
I_printkeys (void)
{
  if (mt_gotoXY (13, 48))
    return ERROR_CODE;
  sc_print (" Keys ");
  if (mt_gotoXY (14, 48))
    return ERROR_CODE;
  sc_print ("l - load\n");
  if (mt_gotoXY (15, 48))
    return ERROR_CODE;
  sc_print ("s - save\n");
  if (mt_gotoXY (16, 48))
    return ERROR_CODE;
  sc_print ("r - run\n");
  if (mt_gotoXY (17, 48))
    return ERROR_CODE;
  sc_print ("t - step\n");
  if (mt_gotoXY (18, 48))
    return ERROR_CODE;
  sc_print ("F5 - accumulator\n");
  if (mt_gotoXY (19, 48))
    return ERROR_CODE;
  sc_print ("F6 - instructionCounter\n");
  return SUCCES_CODE;
}

int
I_printaccumulator ()
{
  if (mt_gotoXY (1, 67))
    return ERROR_CODE;
  sc_print (" accumulator ");
  if (mt_gotoXY (I_POS_ACCUMULATOR_X, I_POS_ACCUMULATOR_Y))
    return ERROR_CODE;

  accumulator & 0x4000 ? sc_print ("-") : sc_print ("+");
  sc_print ("%04X", accumulator & 0x3fff);
  return SUCCES_CODE;
}

int
I_printinstructionCounter ()
{
  if (mt_gotoXY (4, 64))
    return ERROR_CODE;
  sc_print (" instructionCounter ");
  if (mt_gotoXY (I_POS_IC_X, I_POS_IC_Y))
    return ERROR_CODE;
  sc_print ("+%04X", instructionCounter);
  return SUCCES_CODE;
}

int
I_printoperations ()
{
  if (mt_gotoXY (7, 68))
    return ERROR_CODE;
  sc_print (" Operation ");
  if (mt_gotoXY (I_POS_OPERATION_X, I_POS_OPERATION_Y))
    return ERROR_CODE;

  int t1, t2, val;
  if (sc_memoryGet (instructionCounter, &val))
    return ERROR_CODE;
  if (sc_commandDecode (val, &t1, &t2))
    return ERROR_CODE;
  sc_print ("+%02X : %02X", t1, t2);
  return SUCCES_CODE;
}

int
I_printflags ()
{
  if (mt_gotoXY (10, 70))
    return ERROR_CODE;
  sc_print (" Flags ");
  if (mt_gotoXY (I_POS_FLAGS_X, I_POS_FLAGS_Y))
    return ERROR_CODE;
  int r = 0, c;
  r |= sc_regGet (err_overflow, &c);
  c ? sc_print ("O ") : sc_print (" ");
  r |= sc_regGet (err_division_by_zero, &c);
  c ? sc_print ("D ") : sc_print (" ");
  r |= sc_regGet (err_out_of_range, &c);
  c ? sc_print ("R ") : sc_print (" ");
  r |= sc_regGet (err_ignoring_clock_pulses, &c);
  c ? sc_print ("I ") : sc_print (" ");
  r |= sc_regGet (err_invalid_command, &c);
  c ? sc_print ("C ") : sc_print (" ");
  return r;
}

int
I_printcustomfields ()
{
  // bc_box(23, 1, 25, 46) || bc_box(23, 47, 25, 84)
  if (mt_gotoXY (23, 3))
    return ERROR_CODE;
  sc_print (" Input ");
  if (mt_gotoXY (24, 2))
    return ERROR_CODE;
  sc_print (" > ");
  if (mt_gotoXY (23, 49))
    return ERROR_CODE;
  sc_print (" Output ");
  return SUCCES_CODE;
}

int
I_printInputField (bool status, const char *format, ...)
{
  if (mt_gotoXY (24, 2))
    return ERROR_CODE;
  for (int i = 1; i < 45; ++i)
    sc_print (" ");
  if (mt_gotoXY (24, 2))
    return ERROR_CODE;
  status ? sc_print (" \e[31m>\e[39m ") : sc_print (" > ");
  if (format)
    {
      va_list ap;
      va_start (ap, format);
      vprintf (format, ap);
    }
  return SUCCES_CODE;
}

int
I_printOutputField (const char *format, ...)
{
  if (outpitLine > 8)
    outpitLine = 0;
  if (mt_gotoXY (24 + outpitLine, 49))
    return ERROR_CODE;
  for (int i = 49; i < 84; ++i)
    sc_print (" ");
  if (mt_gotoXY (24 + outpitLine, 49))
    return ERROR_CODE;
  if (format)
    {
      va_list ap;
      va_start (ap, format);
      vprintf (format, ap);
      ++outpitLine;
    }
  return SUCCES_CODE;
}

int
I_setAccumulator ()
{
  I_printInputField (1, "Accumulator: ");
  int c1;
  scanf ("%d", &c1);
  __fpurge (stdin);
  I_printInputField (0, NULL);
  if (c1 < 0)
    accumulator = ((-c1) & 0x3fff) | 0x4000;
  else
    accumulator = c1 & 0x3fff;
  I_printInputField (0, NULL);
  return I_printaccumulator ();
}

int
I_setInstructionCounter (void)
{
  I_printInputField (1, " InstructionCounter: ");
  int c;
  scanf ("%d", &c);
  I_printInputField (0, NULL);
  __fpurge (stdin);
  if (c > 0x63 || c < 0)
    {
      sc_regSet (err_out_of_range, 1);
      return ERROR_CODE;
    }
  if (I_printhex (instructionCounter, color_default, color_default))
    return ERROR_CODE;
  instructionCounter = (int)c;
  I_move_address_xy (5);
  return I_printinstructionCounter ();
}

int
I_runprogram ()
{
  struct itimerval nval, oval;
  nval.it_interval.tv_sec = 0;
  nval.it_interval.tv_usec = 100000;
  nval.it_value.tv_sec = 0;
  nval.it_value.tv_usec = 100000;
  rignore = 0;
  return setitimer (ITIMER_REAL, &nval, &oval);
}

int
I_restartsc (void)
{
  if (sc_memoryFree () || sc_memoryInit () || sc_regInit ())
    return ERROR_CODE;
  struct itimerval nval, oval;
  nval.it_interval.tv_sec = 0;
  nval.it_interval.tv_usec = 0;
  nval.it_value.tv_sec = 0;
  nval.it_value.tv_usec = 0;
  startcu = false;
  rignore = 1;
  I_moveInstructionCounter (0);
  return setitimer (ITIMER_REAL, &nval, &oval) || I_printall ()
         || sc_regInit ();
}

void
I_sigusr1 (int sig)
{
  lastsig = sig;
  I_restartsc ();
}

int
I_stopprogram (void)
{
  struct itimerval nval, oval;
  nval.it_interval.tv_sec = 0;
  nval.it_interval.tv_usec = 0;
  nval.it_value.tv_sec = 0;
  nval.it_value.tv_usec = 0;
  startcu = false;
  rignore = 1;
  halt_ex = true;
  // alarm(0);
  return setitimer (ITIMER_REAL, &nval, &oval);
}

int
I_moveInstructionCounter (int ic)
{
  I_printInputField (1, " InstructionCounter: ");
  int c = ic;
  if (c > 0x63 || c < 0)
    {
      sc_regSet (err_out_of_range, 1);
      return ERROR_CODE;
    }
  if (I_printhex (instructionCounter, color_default, color_default))
    return ERROR_CODE;
  instructionCounter = c;
  I_move_address_xy (5);
  I_printInputField (0, NULL);
  return I_printinstructionCounter ();
}
