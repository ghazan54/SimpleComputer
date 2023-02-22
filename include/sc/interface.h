#pragma once

#include <sc/bigchar.h>
#include <sc/ram-operations.h>
#include <sc/terminal.h>

#define I_POS_ACCUMULATOR_X 2
#define I_POS_ACCUMULATOR_Y 71
#define I_POS_IC_X 5
#define I_POS_IC_Y 71
#define I_POS_OPERATION_X 8
#define I_POS_OPERATION_Y 71
#define I_POS_FLAGS_X 11
#define I_POS_FLAGS_Y 69

int I_simplecomputer(void);
int I_printall(void);
int I_printhex(int x, int y, enum colors fg, enum colors bg);
int I_printinfo(const char I, enum colors fg, enum colors bg);
int I_printbig(int x, int y);
int I_printkeys(void);
int I_printaccumulator(void);
int I_printinstructionCounter(void);
int I_printoperations(void);
int I_printflags(void);
int I_startsc(void);
