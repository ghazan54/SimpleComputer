#pragma once

#define KEY_F5 "\e[15~"
#define KEY_F6 "\e[17~"
#define KEY_UP "\e[A"
#define KEY_DOWN "\e[B"
#define KEY_RIGHT "\e[C"
#define KEY_LEFT "\e[D"

enum keys {
    key_L,
    key_S,
    key_R,
    key_T,
    key_I,
    key_F5,
    key_F6,
    key_UP,
    key_DOWN,
    key_RIGHT,
    key_LEFT,
    key_ENTER
};

int rk_readkey(enum keys* key);
int rk_keyaction(const enum keys key);
int rk_mytermsave(void);
int rk_mytermrestore(void);
int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint);
