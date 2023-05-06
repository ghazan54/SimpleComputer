#include <ctest.h>
#include <sc/helper.h>
#include <simple_computer.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

CTEST (keys, rk_mytermsave) { ASSERT_EQUAL (SUCCES_CODE, rk_mytermsave ()); }

CTEST (keys, rk_mytermrestore)
{
  ASSERT_EQUAL (SUCCES_CODE, rk_mytermrestore ());
}

CTEST (keys, rk_mytermregime)
{
  ASSERT_EQUAL (SUCCES_CODE, rk_mytermsave ());
  ASSERT_EQUAL (SUCCES_CODE, rk_mytermregime (0, 0, 1, 0, 0));
  ASSERT_EQUAL (SUCCES_CODE, rk_mytermregime (1, 0, 0, 1, 1));
  ASSERT_EQUAL (SUCCES_CODE, rk_mytermrestore ());
}

CTEST (keys, rk_readkey)
{
  freopen ("tmp.txt", "r", stdin);
  enum keys key;
  ASSERT_EQUAL (ERROR_CODE, rk_readkey (&key));
}
