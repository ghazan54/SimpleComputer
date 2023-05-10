#include <sc/ALU.h>
#include <sc/CU.h>
#include <sc/helper.h>
#include <sc/interface.h>
#include <sc/ram-operations.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
ALU (int command, int operand)
{
  switch (command)
    {
    case 0x30:
      return alu_add (operand);
    case 0x31:
      return alu_sub (operand);
    case 0x32:
      return alu_divide (operand);
    case 0x33:
      return alu_mul (operand);
    case 0x62:
      return alu_rcl (operand);
    default:
      sc_regSet (err_invalid_command, 1);
      sc_regSet (err_ignoring_clock_pulses, 1);
      break;
    }
  return ERROR_CODE;
}

int
alu_add (int operand)
{
  int val;
  if (sc_memoryGet (operand, &val))
    return ERROR_CODE;
  val = val & 0x4000 ? -(val & 0x3fff) : val & 0x3fff;
  bool sign = accumulator & 0x4000 ? true : false;
  accumulator &= 0x3fff;
  accumulator = sign ? -accumulator : accumulator;
  long long tmp = (long long)accumulator + (long long)val;
  if (tmp > 0x3fff || tmp < -0x3fff)
    {
      accumulator = 0;
      sc_regSet (err_overflow, 1);
    }
  else
    {
      accumulator = (int)tmp;
    }

  if (accumulator < 0)
    {
      accumulator = abs (accumulator);
      accumulator |= 0x4000;
    }
  else
    {
      accumulator = abs (accumulator);
      accumulator &= 0x3fff;
    }
  return I_printaccumulator ();
}

int
alu_sub (int operand)
{
  int val;
  if (sc_memoryGet (operand, &val))
    return ERROR_CODE;
  val = val & 0x4000 ? -(val & 0x3fff) : val & 0x3fff;
  bool sign = accumulator & 0x4000 ? true : false;
  accumulator &= 0x3fff;
  accumulator = sign ? -accumulator : accumulator;
  long long tmp = (long long)accumulator - (long long)val;
  if (tmp > 0x3fff || tmp < -0x3fff)
    {
      accumulator = 0;
      sc_regSet (err_overflow, 1);
    }
  else
    {
      accumulator = (int)tmp;
    }

  if (accumulator < 0)
    {
      accumulator = abs (accumulator);
      accumulator |= 0x4000;
    }
  else
    {
      accumulator = abs (accumulator);
      accumulator &= 0x3fff;
    }
  return I_printaccumulator ();
}

int
alu_divide (int operand)
{
  int val;
  if (sc_memoryGet (operand, &val))
    return ERROR_CODE;
  val = val & 0x4000 ? -(val & 0x3fff) : val & 0x3fff;
  bool sign = accumulator & 0x4000 ? true : false;
  accumulator &= 0x3fff;
  accumulator = sign ? -accumulator : accumulator;
  if (!val)
    {
      sc_regSet (err_division_by_zero, 1);
      return ERROR_CODE;
    }

  accumulator /= val;

  if (accumulator < 0)
    {
      accumulator = abs (accumulator);
      accumulator |= 0x4000;
    }
  else
    {
      accumulator = abs (accumulator);
      accumulator &= 0x3fff;
    }
  return I_printaccumulator ();
}

int
alu_mul (int operand)
{
  int val;
  if (sc_memoryGet (operand, &val))
    return ERROR_CODE;
  val = val & 0x4000 ? -(val & 0x3fff) : val & 0x3fff;
  bool sign = accumulator & 0x4000 ? true : false;
  accumulator &= 0x3fff;
  accumulator = sign ? -accumulator : accumulator;
  long long tmp = (long long)accumulator * (long long)val;
  if (tmp > 0x3fff || tmp < -0x3fff)
    {
      accumulator = 0;
      sc_regSet (err_overflow, 1);
    }
  else
    {
      accumulator *= val;
    }

  if (accumulator < 0)
    {
      accumulator = abs (accumulator);
      accumulator |= 0x4000;
    }
  else
    {
      accumulator = abs (accumulator);
      accumulator &= 0x3fff;
    }
  return I_printaccumulator ();
}

int
alu_rcl (int operand)
{
  int val;
  if (sc_memoryGet (operand, &val))
    return ERROR_CODE;
  bool sign = val & 0x4000;
  int msb = (val >> 13) & 1;
  val <<= 1;
  val |= msb;
  val &= 0x3fff;
  accumulator = sign ? val | 0x4000 : val;
  return I_printaccumulator ();
}
