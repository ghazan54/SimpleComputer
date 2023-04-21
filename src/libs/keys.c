#include <sc/CU.h>
#include <sc/interface.h>
#include <sc/keys.h>
#include <sc/ram-operations.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

static struct termios term_settings;

int rk_readkey(enum keys* key)
{
    char bf[6] = { 0 };
    if (rk_mytermsave())
        return EXIT_FAILURE;
    rk_mytermregime(0, 0, 1, 0, 1);
    if (read(STDIN_FILENO, bf, 5) == -1) {
        rk_mytermrestore();
        return EXIT_FAILURE;
    }
    if (rk_mytermrestore())
        return EXIT_FAILURE;
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
    } else if (!strcmp("\n", bf)) {
        *key = key_ENTER;
    } else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int rk_keyaction(const enum keys key)
{
    switch (key) {
    case key_L:
        return sc_memoryLoad(SAVE_PATH) || I_printall();
    case key_S:
        mkdir("data", S_IRWXU | S_IRWXG | S_IRWXO);
        return sc_memorySave(SAVE_PATH);
    case key_R:
        startcu = 2;
        return EXIT_SUCCESS;
    case key_T:
        startcu = 1;
        return EXIT_SUCCESS;
    case key_I:
        return I_restartsc();
    case key_F5:
        return I_setAccumulator();
    case key_F6:
        return I_setInstructionCounter();
    case key_UP:
        return I_move_address_xy(0);
    case key_DOWN:
        return I_move_address_xy(1);
    case key_RIGHT:
        return I_move_address_xy(2);
    case key_LEFT:
        return I_move_address_xy(3);
    case key_ENTER:
        return cu_read(cur_x * DEFAULT_MAX_STRS + cur_y) || I_printhex(cur_x, cur_y, color_red, color_default);
    default:
        // I_printOutputField("Unknow key");
        break;
    }
    return EXIT_FAILURE;
}

int rk_mytermsave(void)
{
    return tcgetattr(STDIN_FILENO, &term_settings) != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int rk_mytermrestore(void)
{
    return tcsetattr(STDIN_FILENO, TCSANOW, &term_settings) != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint)
{
    struct termios t;
    if (tcgetattr(STDIN_FILENO, &t) == -1)
        return EXIT_FAILURE;
    if (regime) {
        t.c_lflag &= ICANON | ECHO;
    } else {
        t.c_lflag &= ~ICANON;
        t.c_lflag = echo ? t.c_lflag | ECHO : t.c_lflag & (~ECHO);
        t.c_lflag = sigint ? t.c_lflag | ISIG : t.c_lflag & (~ISIG);
        t.c_cc[VMIN] = vmin;
        t.c_cc[VTIME] = vtime;
    }
    return tcsetattr(STDIN_FILENO, TCSANOW, &t) != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}
