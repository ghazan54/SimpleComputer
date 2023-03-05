#include <ctest.h>
#include <simple_computer.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

CTEST(terminal, mt_clrscr) { ASSERT_EQUAL(EXIT_SUCCESS, mt_clrscr()); }

CTEST(terminal, mt_gotoXY) {
    ASSERT_EQUAL(EXIT_SUCCESS, mt_gotoXY(3, 1));
    ASSERT_EQUAL(EXIT_FAILURE, mt_gotoXY(-1, 2));
    ASSERT_EQUAL(EXIT_FAILURE, mt_gotoXY(2, -1));
    ASSERT_EQUAL(EXIT_FAILURE, mt_gotoXY(-1, -1));
}

CTEST(terminal, mt_getscreensize) {
    int a, b;
    ASSERT_EQUAL(EXIT_SUCCESS, mt_getscreensize(&a, &b));
    ASSERT_EQUAL(EXIT_FAILURE, mt_getscreensize(NULL, &b));
    ASSERT_EQUAL(EXIT_FAILURE, mt_getscreensize(&a, NULL));
    ASSERT_EQUAL(EXIT_FAILURE, mt_getscreensize(NULL, NULL));
}

CTEST(terminal, mt_setfgcolor) { ASSERT_EQUAL(EXIT_SUCCESS, mt_setfgcolor(color_green)); }

CTEST(terminal, mt_setbgcolor) { ASSERT_EQUAL(EXIT_SUCCESS, mt_setbgcolor(color_green)); }