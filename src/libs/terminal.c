#include <sc/terminal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

int mt_clrscr(void)
{
    return printf(TERM_CLEAR_SCREEN) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int mt_gotoXY(int x, int y)
{
    if (x < 0 || y < 0)
        return EXIT_FAILURE;
    return printf("\e[%d;%df", y, x) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int mt_getscreensize(int* rows, int* cols)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) || !rows || !cols) {
        return EXIT_FAILURE;
    } else {
        *rows = ws.ws_row;
        *cols = ws.ws_col;
        return EXIT_SUCCESS;
    }
}

int mt_setfgcolor(enum colors color)
{
    return printf("\e[3%dm", color) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int mt_setbgcolor(enum colors color)
{
    return printf("\e[4%dm", color) ? EXIT_SUCCESS : EXIT_FAILURE;
}
