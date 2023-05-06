#include <fcntl.h>
#include <sc/bigchar.h>
#include <sc/helper.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
bc_printA (const char *str)
{
  return str && !sc_print ("\e(0%s\e(B", str) ? SUCCES_CODE : ERROR_CODE;
}

int
bc_box (int x1, int y1, const int x2, const int y2)
{
  if (x1 > x2 || y1 > y2 || x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0)
    return ERROR_CODE;
  bool first = true;
  for (; x1 <= x2; ++x1, first = false)
    {
      if (mt_gotoXY (x1, y1))
        return ERROR_CODE;
      if (first)
        {
          for (int _y1 = y1; _y1 <= y2; ++_y1)
            {
              if (_y1 == y1)
                {
                  if (bc_printA (ACS_ULCORNER))
                    return ERROR_CODE;
                }
              else if (_y1 == y2)
                {
                  if (bc_printA (ACS_URCORNER))
                    return ERROR_CODE;
                }
              else
                {
                  if (bc_printA (ACS_HLINE))
                    return ERROR_CODE;
                }
            }
        }
      else if (x1 == x2)
        {
          for (int _y1 = y1; _y1 <= y2; ++_y1)
            {
              if (_y1 == y1)
                {
                  if (bc_printA (ACS_LLCORNER))
                    return ERROR_CODE;
                }
              else if (_y1 == y2)
                {
                  if (bc_printA (ACS_LRCORNER))
                    return ERROR_CODE;
                }
              else
                {
                  if (bc_printA (ACS_HLINE))
                    return ERROR_CODE;
                }
            }
        }
      else
        {
          bool firsty = true;
          for (int _y1 = y1; _y1 <= y2; ++_y1, firsty = false)
            {
              firsty || _y1 == y2 ? bc_printA (ACS_VLINE) : sc_print (" ");
            }
        }
      sc_print ("\n");
    }
  return SUCCES_CODE;
}

int
bc_printbigchar (const int n1[2], int x, int y, enum colors fg, enum colors bg)
{
  int n[] = { n1[0], n1[1] };
  if (mt_setfgcolor (fg))
    return ERROR_CODE;
  if (mt_setbgcolor (bg))
    return ERROR_CODE;
  for (int i = 0; i < 2; ++i)
    {
      for (int j = 0; j < 4; ++j)
        {
          if (mt_gotoXY (x++, y))
            return ERROR_CODE;
          for (int k = 0; k < 8; ++k)
            {
              int bit = n[i] & 0x1;
              n[i] >>= 1;
              bit ? bc_printA (ACS_CKBOARD) : sc_print (" ");
            }
          sc_print ("\n");
        }
    }
  if (mt_gotoXY (++x, y))
    return ERROR_CODE;
  return SUCCES_CODE;
}

int
bc_setbigcharpos (int *big, int x, int y, int value)
{
  if (x < 0 || y < 0 || x > 7 || y > 7)
    {
      return ERROR_CODE;
    }

  int i;
  if (x >= 4)
    {
      i = 1;
      x -= 4;
    }
  else
    {
      i = 0;
    }
  if (value)
    {
      big[i] |= 1 << (x * 8 + y);
    }
  else
    {
      big[i] &= ~(1 << (x * 8 + y));
    }

  return SUCCES_CODE;
}

int
bc_getbigcharpos (int *big, int x, int y, int *value)
{
  if (x < 0 || y < 0 || x > 7 || y > 7 || !value)
    {
      return ERROR_CODE;
    }

  int i;
  if (x >= 4)
    {
      i = 1;
      x -= 4;
    }
  else
    {
      i = 0;
    }

  *value = (big[i] >> (x * 8 + y)) & 1;
  return SUCCES_CODE;
}

int
bc_bigcharwrite (int fd, int *big, int count)
{
  return !big || write (fd, big, count * sizeof (int) * 2) == -1 ? ERROR_CODE
                                                                 : SUCCES_CODE;
}

int
bc_bigcharread (int fd, int *big, int need_count, int *count)
{
  if (!big || !count)
    return ERROR_CODE;
  *count = read (fd, big, need_count * sizeof (int) * 2);
  if (*count != -1)
    *count = *count / sizeof (int) / 2;
  return !big || *count != need_count ? ERROR_CODE : SUCCES_CODE;
}
