#include <fcntl.h>
#include <sc/terminal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int mt_clrscr(void) {
    int term;
    if ((term = open(TERMINAL, O_WRONLY)) == -1) {
        return EXIT_FAILURE;
    }
    int c = write(term, TERM_CLEAR_SCREEN, sizeof(TERM_CLEAR_SCREEN));
    close(term);
    return c == sizeof(TERM_CLEAR_SCREEN) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int mt_gotoXY(int x, int y) {
    if (x < 0 || y < 0) {
        return EXIT_FAILURE;
    }
    int term;
    if ((term = open(TERMINAL, O_WRONLY)) == -1) {
        return EXIT_FAILURE;
    }
    char s[BUFSIZ] = {0};
    sprintf(s, "\e[%d;%df", x, y);
    int c = write(term, s, BUFSIZ);
    close(term);
    return c != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int mt_getscreensize(int* rows, int* cols) {
    struct winsize ws;
    if (!rows || !cols || ioctl(1, TIOCGWINSZ, &ws)) {
        return EXIT_FAILURE;
    } else {
        *rows = ws.ws_row;
        *cols = ws.ws_col;
        return EXIT_SUCCESS;
    }
}

int mt_setfgcolor(enum colors color) {
    int term;
    if ((term = open(TERMINAL, O_WRONLY)) == -1) {
        return EXIT_FAILURE;
    }
    char s[BUFSIZ] = {0};
    sprintf(s, "\e[3%dm", color);
    int c = write(term, s, BUFSIZ);
    close(term);
    return c != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int mt_setbgcolor(enum colors color) {
    int term;
    if ((term = open(TERMINAL, O_WRONLY)) == -1) {
        return EXIT_FAILURE;
    }
    char s[BUFSIZ] = {0};
    sprintf(s, "\e[4%dm", color);
    int c = write(term, s, BUFSIZ);
    close(term);
    return c != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}
