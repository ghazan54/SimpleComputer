
#include <errno.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <net/if.h>
#include <sc/terminal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int mt_clrscr(void)
{
    int term;
    if ((term = open(TERMINAL, O_WRONLY)) == -1) {
        return EXIT_FAILURE;
    }
    int c = write(term, "\e[H\e[J", strlen("\e[H\e[J"));
    close(term);
    return c != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int mt_getscreensize(int* rows, int* columns)
{
    struct winsize ws;
    if (!rows || !columns || ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)) {
        return EXIT_FAILURE;
    } else {
        *rows = ws.ws_row;
        *columns = ws.ws_col;
        return EXIT_SUCCESS;
    }
}

int mt_gotoXY(int x, int y)
{
    int row, col;
    mt_getscreensize(&row, &col);
    if (((x > col) || (x < 0)) || ((y > row) || (y < 0))) {
        return EXIT_FAILURE;
    }
    char s[BUFSIZ];
    sprintf(s, "\e[%d;%dH", x, y);
    int term;
    if ((term = open(TERMINAL, O_WRONLY)) == -1) {
        return EXIT_FAILURE;
    }
    int c = write(term, s, strlen(s));
    close(term);
    return c != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int mt_setfgcolor(enum colors color)
{
    char s[BUFSIZ];
    sprintf(s, "\e[0;3%dm", color);
    int term;
    if ((term = open(TERMINAL, O_WRONLY)) == -1) {
        return EXIT_FAILURE;
    }
    int c = write(term, s, strlen(s));
    close(term);
    return c != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int mt_setbgcolor(enum colors color)
{
    char s[BUFSIZ];
    sprintf(s, "\e[4%dm", color);
    int term;
    if ((term = open(TERMINAL, O_WRONLY)) == -1) {
        return EXIT_FAILURE;
    }
    int c = write(term, s, strlen(s));
    close(term);
    return c != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}
