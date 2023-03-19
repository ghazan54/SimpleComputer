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
    freopen("tmp.txt", "r", stdin);
    enum keys key;
    ASSERT_EQUAL(EXIT_FAILURE, rk_readkey(&key));
}
