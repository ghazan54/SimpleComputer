#include <sc/bigchar.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>

int bc_printA(const char* str) { return str && printf("\e(0%s\e(B", str) ? EXIT_SUCCESS : EXIT_FAILURE; }

int bc_box(int x1, int y1, const int x2, const int y2) {
    if (x1 > x2 || y1 > y2 || x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0) return EXIT_FAILURE;
    // if (mt_clrscr())
    //     return EXIT_FAILURE;
    bool first = true;
    for (; y1 <= y2; ++y1, first = false) {
        if (mt_gotoXY(x1, y1)) return EXIT_FAILURE;
        if (first) {
            for (int _x1 = x1; _x1 <= x2; ++_x1) {
                if (_x1 == x1) {
                    if (bc_printA(ACS_ULCORNER)) return EXIT_FAILURE;
                } else if (_x1 == x2) {
                    if (bc_printA(ACS_URCORNER)) return EXIT_FAILURE;
                } else {
                    if (bc_printA(ACS_HLINE)) return EXIT_FAILURE;
                }
            }
        } else if (y1 == y2) {
            for (int _x1 = x1; _x1 <= x2; ++_x1) {
                if (_x1 == x1) {
                    if (bc_printA(ACS_LLCORNER)) return EXIT_FAILURE;
                } else if (_x1 == x2) {
                    if (bc_printA(ACS_LRCORNER)) return EXIT_FAILURE;
                } else {
                    if (bc_printA(ACS_HLINE)) return EXIT_FAILURE;
                }
            }
        } else {
            bool firstx = true;
            for (int _x1 = x1; _x1 <= x2; ++_x1, firstx = false) {
                firstx || _x1 == x2 ? bc_printA(ACS_VLINE) : printf(" ");
            }
        }
        printf("\n");
    }
    return EXIT_SUCCESS;
}

int bc_printbigchar(int n[2], int x, int y, enum colors fg, enum colors bg) {
    // mt_clrscr();

    if (mt_setfgcolor(fg)) return EXIT_FAILURE;

    if (mt_setbgcolor(bg)) return EXIT_FAILURE;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (mt_gotoXY(x, y++)) return EXIT_FAILURE;
            for (int k = 0; k < 8; ++k) {
                int bit = n[i] & 0x1;
                n[i] >>= 1;
                bit ? bc_printA(ACS_CKBOARD) : printf(" ");
            }
            printf("\n");
        }
    }
    if (mt_gotoXY(x, ++y)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

// int bc_setbigcharpos(int* big, int x, int y, int value)
// {
// }

// int bc_getbigcharpos(int* big, int x, int y, int* value)
// {
// }

// int bc_bigcharwrite(int fd, int* big, int count)
// {
// }

// int bc_bigcharread(int fd, int* big, int need_count, int* count)
// {
// }