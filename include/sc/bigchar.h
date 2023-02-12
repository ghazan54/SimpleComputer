#pragma once

#include <sc/terminal.h>

#define ACS_CKBOARD ("a")

int bc_printA(const char* str);
int bc_box(int x1, int y1, const int x2, const int y2);
int bc_printbigchar(int n[2], int x, int y, enum colors fg, enum colors bg);
int bc_setbigcharpos(int* big, int x, int y, int value);
int bc_getbigcharpos(int* big, int x, int y, int* value);
int bc_bigcharwrite(int fd, int* big, int count);
int bc_bigcharread(int fd, int* big, int need_count, int* count);
