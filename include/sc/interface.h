#pragma once

#include <sc/bigchar.h>
#include <sc/ram-operations.h>
#include <sc/terminal.h>

int I_simplecomputer(void);
int I_printall(void);
int I_printhex(int x, int y, enum colors fg, enum colors bg);
