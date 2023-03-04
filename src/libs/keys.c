#include <sc/interface.h>
#include <sc/keys.h>
#include <sc/ram-operations.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

static struct termios term_settings;

int rk_readkey(enum keys* key) {
    char bf[5] = {0};
    if (read(STDIN_FILENO, bf, 4) == -1) return EXIT_FAILURE;
    if (!strcmp("l", bf)) {
        *key = key_L;
    } else if (!strcmp("s", bf)) {
        *key = key_S;
    } else if (!strcmp("r", bf)) {
        *key = key_R;
    } else if (!strcmp("t", bf)) {
        *key = key_T;
    } else if (!strcmp("i", bf)) {
        *key = key_I;
    } else if (!strcmp(KEY_F5, bf)) {
        *key = key_F5;
    } else if (!strcmp(KEY_F6, bf)) {
        *key = key_F6;
    } else if (!strcmp(KEY_UP, bf)) {
        *key = key_UP;
    } else if (!strcmp(KEY_DOWN, bf)) {
        *key = key_DOWN;
    } else if (!strcmp(KEY_RIGHT, bf)) {
        *key = key_RIGHT;
    } else if (!strcmp(KEY_LEFT, bf)) {
        *key = key_LEFT;
    } else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int rk_keyaction(const enum keys key) {
    int res = 0;
    switch (key) {
        case key_L:
            break;
        case key_S:
            break;
        case key_R:
            break;
        case key_T:
            break;
        case key_I:
            break;
        case key_F5:
            break;
        case key_F6:
            break;
        case key_UP:
            break;
        case key_DOWN:
            break;
        case key_RIGHT:
            break;
        case key_LEFT:
            break;
        default:
            return EXIT_FAILURE;
    }
    return res;
}

int rk_mytermsave(void) {
    return tcgetattr(STDIN_FILENO, &term_settings) != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int rk_mytermrestore(void) {
    return tcsetattr(STDIN_FILENO, TCSANOW, &term_settings) != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint) {
    struct termios t;
    if (tcgetattr(STDIN_FILENO, &t) == -1) return EXIT_FAILURE;
    if (regime) {
        t.c_lflag |= ICANON;
    } else {
        t.c_lflag |= ~ICANON;
        t.c_cc[VTIME] = vtime;
        t.c_cc[VMIN] = vmin;
        t.c_lflag |= echo ? ECHO : ~ECHO;
        t.c_lflag |= sigint ? ISIG : ~ISIG;
    }
    return tcsetattr(STDIN_FILENO, TCSANOW, &t) != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}
