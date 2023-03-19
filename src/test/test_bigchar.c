#include <ctest.h>
#include <fcntl.h>
#include <simple_computer.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

CTEST(bigchar, bc_printA) {
    // FILE* f = freopen("tmp.tmp", "w", stdout);
    ASSERT_EQUAL(EXIT_SUCCESS, bc_printA("abcd"));
    ASSERT_EQUAL(EXIT_FAILURE, bc_printA(NULL));
    //     fclose(f);
    //     freopen("/dev/tty", "w", stdout);
    //     remove("tmp.tmp");
}

CTEST(bigchar, bc_box) {
    ASSERT_EQUAL(EXIT_SUCCESS, bc_box(5, 5, 10, 10));
    ASSERT_EQUAL(EXIT_FAILURE, bc_box(11, 5, 10, 10));
    ASSERT_EQUAL(EXIT_FAILURE, bc_box(5, 11, 10, 10));
    ASSERT_EQUAL(EXIT_FAILURE, bc_box(-1, 5, 10, 10));
    ASSERT_EQUAL(EXIT_FAILURE, bc_box(5, -1, 10, 10));
}

CTEST(bigchar, bc_printbigchar) {
    int n[] = {0, 0};
    ASSERT_EQUAL(EXIT_SUCCESS, bc_printbigchar(n, 5, 10, color_default, color_default));
}

CTEST(bigchar, bc_setbigcharpos) {
    int n[] = {0, 0};
    ASSERT_EQUAL(EXIT_SUCCESS, bc_setbigcharpos(n, 1, 1, 1));
    ASSERT_EQUAL(EXIT_SUCCESS, bc_setbigcharpos(n, 5, 1, 1));
    ASSERT_EQUAL(EXIT_FAILURE, bc_setbigcharpos(n, -1, 1, 1));
    ASSERT_EQUAL(EXIT_FAILURE, bc_setbigcharpos(n, 1, -1, 1));
    ASSERT_EQUAL(EXIT_FAILURE, bc_setbigcharpos(n, 8, 1, 1));
    ASSERT_EQUAL(EXIT_FAILURE, bc_setbigcharpos(n, 1, 8, 1));
}

CTEST(bigchar, bc_getbigcharpos) {
    int n[] = {0, 0};
    int v;
    ASSERT_EQUAL(EXIT_SUCCESS, bc_getbigcharpos(n, 1, 1, &v));
    ASSERT_EQUAL(0, v);
    ASSERT_EQUAL(EXIT_SUCCESS, bc_getbigcharpos(n, 5, 1, &v));
    ASSERT_EQUAL(0, v);
    ASSERT_EQUAL(EXIT_FAILURE, bc_getbigcharpos(n, -1, 1, &v));
    ASSERT_EQUAL(0, v);
    ASSERT_EQUAL(EXIT_FAILURE, bc_getbigcharpos(n, 1, -1, &v));
    ASSERT_EQUAL(0, v);
    ASSERT_EQUAL(EXIT_FAILURE, bc_getbigcharpos(n, 8, 1, &v));
    ASSERT_EQUAL(0, v);
    ASSERT_EQUAL(EXIT_FAILURE, bc_getbigcharpos(n, 1, 8, &v));
    ASSERT_EQUAL(0, v);
}

CTEST(bigchar, bc_bigcharwrite) {
    int fd = creat("tmp.tmp", S_IREAD | S_IWRITE);
    int n[] = {0, 0};
    ASSERT_EQUAL(EXIT_SUCCESS, bc_bigcharwrite(fd, n, 1));
    close(fd);
}

CTEST(bigchar, bc_bigcharread) {
    int fd = open("tmp.tmp", O_RDONLY);
    int n[] = {0, 0};
    int v;
    ASSERT_EQUAL(EXIT_SUCCESS, bc_bigcharread(fd, n, 1, &v));
    ASSERT_EQUAL(1, v);
    remove("tmp.tmp");
}
