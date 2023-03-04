#include <ctest.h>
#include <simple_computer.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

CTEST(terminal, mt_clrscr) { ASSERT_EQUAL(EXIT_SUCCESS, mt_clrscr()); }
