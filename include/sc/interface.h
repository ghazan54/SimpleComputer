#pragma once

#include <sc/bigchar.h>
#include <sc/keys.h>
#include <sc/ram-operations.h>
#include <sc/terminal.h>
#include <stdarg.h>
#include <stdbool.h>

#define I_POS_ACCUMULATOR_X 2
#define I_POS_ACCUMULATOR_Y 71
#define I_POS_IC_X 5
#define I_POS_IC_Y 71
#define I_POS_OPERATION_X 8
#define I_POS_OPERATION_Y 70
#define I_POS_FLAGS_X 11
#define I_POS_FLAGS_Y 69

#define READ_WAIT (4)

extern int cur_x;
extern int cur_y;

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
int I_scstep(int rignore);
int I_printcustomfields(void);
int I_printInputField(bool status, const char* format, ...);
int I_printOutputField(const char* format, ...);
int I_move_address_xy(const int d);
int I_executeOperation(void);
int I_setAccumulator(void);
int I_setInstructionCounter(void);
int I_ignoreimp(void);
int I_restartsc(void);
long long xtoll(char* s);

//* sighandler

void I_stopsc(int sig);
void I_sigalarm(int sig);
void I_sigusr1(int sig);
