#pragma once

#include <sc/terminal.h>

#define ACS_CKBOARD ("a")
#define ACS_HLINE ("q")
#define ACS_VLINE ("x")
#define ACS_LLCORNER ("m")
#define ACS_LRCORNER ("j")
#define ACS_ULCORNER ("l")
#define ACS_URCORNER ("k")

// #define BC_NIL_1 (0x8181817E)
// #define BC_NIL_2 (0x7E818181)
// #define BC_ONE_1 (0x40485060)
// #define BC_ONE_2 (0x40404040)
// #define BC_TWO_1 (0x2040423C)
// #define BC_TWO_2 (0x7E040810)
// #define BC_THREE_1 (0x7C40407C)
// #define BC_THREE_2 (0x7C404040)
// #define BC_FOUR_1 (0x7E424242)
// #define BC_FOUR_2 (0x40404040)
// #define BC_FIVE_1 (0x7E02027E)
// #define BC_FIVE_2 (0x7E404040)
// #define BC_SIX_1 (0x7E02027E)
// #define BC_SIX_2 (0x7E424242)
// #define BC_SEVEN_1 (0x4040407E)
// #define BC_SEVEN_2 (0x40404040)
// #define BC_EIGHT_1 (0x7E42427E)
// #define BC_EIGHT_2 (0x7E424242)
// #define BC_NINE_1 (0x7E42427E)
// #define BC_NINE_2 (0x7E404040)
// #define BC_PLUS_1 (0xFF181818)
// #define BC_PLUS_2 (0x181818FF)
// #define BC_MINUS_1 (0xFF000000)
// #define BC_MINUS_2 (0xFF)

// extern char big_char_plus[64];
// extern char big_char_minus[64];
// extern char big_char_0[64];
// extern char big_char_1[64];
// extern char big_char_2[64];
// extern char big_char_3[64];
// extern char big_char_4[64];
// extern char big_char_5[64];
// extern char big_char_6[64];
// extern char big_char_7[64];
// extern char big_char_8[64];
// extern char big_char_9[64];
// extern char big_char_A[64];
// extern char big_char_B[64];
// extern char big_char_C[64];
// extern char big_char_D[64];
// extern char big_char_E[64];
// extern char big_char_F[64];

int bc_printA (const char *str);
int bc_box (int x1, int y1, const int x2, const int y2);
int bc_printbigchar (const int n[2], int x, int y, enum colors fg,
                     enum colors bg);
int bc_setbigcharpos (int *big, int x, int y, int value);
int bc_getbigcharpos (int *big, int x, int y, int *value);
int bc_bigcharwrite (int fd, int *big, int count);
int bc_bigcharread (int fd, int *big, int need_count, int *count);
