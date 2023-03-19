#include <ctest.h>
#include <signal.h>
#include <simple_computer.h>

CTEST(signals, I_sigalarm) {
    sc_memoryInit();
    signal(SIGALRM, I_sigalarm);
    raise(SIGALRM);
    ASSERT_EQUAL(SIGALRM, lastsig);
    sc_memoryFree();
    mt_clrscr();
}

CTEST(signals, I_sigusr1) {
    signal(SIGUSR1, I_sigusr1);
    raise(SIGUSR1);
    ASSERT_EQUAL(SIGUSR1, lastsig);
}
