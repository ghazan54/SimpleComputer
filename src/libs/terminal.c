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

static int digit(int n) {
    int c = 0;
    while (n > 0) {
        ++c;
        n /= 10;
    }
    return c;
}

int mt_gotoXY(int x, int y) {
    if (x > 999999 || y > 999999) {
        return EXIT_FAILURE;
    }
    int term;
    if ((term = open(TERMINAL, O_WRONLY)) == -1) {
        return EXIT_FAILURE;
    }
    const int size_s = TERM_GOTO_MIN_SIZE + digit(x) + digit(y) + 1;
    char s[size_s];
    sprintf(s, "%s%d;%d%s", TERM_GOTO_X, x, y, TERM_GOTO_Y);
    int c = write(term, s, size_s);
    close(term);
    return !(c == size_s);
}

int mt_getscreensize(int* rows, int* cols) {
    struct winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws)) {
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
    const int size_s = TERM_FGCOLOR_MIN_SIZE + 2;  // 2 == color_id + \0
    char s[size_s];
    sprintf(s, "%s%d%s", TERM_FGCOLOR_FP, color, TERM_FGCOLOR_SP);
    int c = write(term, s, size_s);
    close(term);
    return !(c == size_s);
}

int mt_setbgcolor(enum colors color) {
    int term;
    if ((term = open(TERMINAL, O_WRONLY)) == -1) {
        return EXIT_FAILURE;
    }
    const int size_s = TERM_BGCOLOR_MIN_SIZE + 2;  // 2 == color_id + \0
    char s[size_s];
    sprintf(s, "%s%d%s", TERM_BGCOLOR_FP, color, TERM_BGCOLOR_SP);
    int c = write(term, s, size_s);
    close(term);
    return !(c == size_s);
}
