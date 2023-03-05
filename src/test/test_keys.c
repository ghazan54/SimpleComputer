#include <ctest.h>
#include <simple_computer.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

CTEST(keys, rk_mytermsave) { ASSERT_EQUAL(EXIT_SUCCESS, rk_mytermsave()); }

CTEST(keys, rk_mytermrestore) { ASSERT_EQUAL(EXIT_SUCCESS, rk_mytermrestore()); }

CTEST(keys, rk_mytermregime) {
    ASSERT_EQUAL(EXIT_SUCCESS, rk_mytermsave());
    ASSERT_EQUAL(EXIT_SUCCESS, rk_mytermregime(0, 0, 1, 0, 0));
    ASSERT_EQUAL(EXIT_SUCCESS, rk_mytermregime(1, 0, 0, 1, 1));
    ASSERT_EQUAL(EXIT_SUCCESS, rk_mytermrestore());
}

CTEST(keys, rk_readkey) {
    FILE* f = fopen("tmp.txt", "w");
    fprintf(f, "\e[15~\n");
    fclose(f);
    freopen("tmp.txt", "r", stdin);
    enum keys key;
    ASSERT_EQUAL(EXIT_SUCCESS, rk_readkey(&key));
    ASSERT_EQUAL(key_F5, key);
    remove("tmp.txt");
}
