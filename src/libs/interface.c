#include <sc/interface.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static int base_x = 2;
static int base_y = 2;

int I_simplecomputer(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    if (mt_clrscr() || sc_memoryInit() || sc_regInit()) return EXIT_FAILURE;
    if (mt_gotoXY(1, 1)) return EXIT_FAILURE;
    if (bc_box(1, 1, 12, 62)) return EXIT_FAILURE;
    if (mt_gotoXY(1, 29)) return EXIT_FAILURE;
    printf(" Memory ");
    sc_memorySet(7, 0x2 | 0x4000);
    I_printall();
    I_printhex(0, 7, color_blue, color_red);

    mt_gotoXY(15, 0);
    return EXIT_SUCCESS;
}

int I_printall(void) {
    for (int i = 0; i < DEFAULT_MAX_STRS; ++i) {
        for (int j = 0; j < DEFAULT_MAX_STRS; ++j) {
            if (I_printhex(i, j, color_default, color_default)) return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int I_printhex(int i, int j, enum colors fg, enum colors bg) {
    if (mt_setfgcolor(fg)) return EXIT_FAILURE;
    if (mt_setbgcolor(bg)) return EXIT_FAILURE;
    if (mt_gotoXY(i + base_x, j ? j * 6 + base_y : base_y)) return EXIT_FAILURE;
    sc_memoryAddressOutput(i, j);
    if (mt_setfgcolor(color_default)) return EXIT_FAILURE;
    if (mt_setbgcolor(color_default)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
