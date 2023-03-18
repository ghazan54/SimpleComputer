#include <sc/CU.h>
#include <sc/interface.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

int operations = 0;
int accumulator = 0;

int CU(int operation) {
    int command, operand;
    if (sc_commandDecode(operation, &command, &operand)) return EXIT_FAILURE;
    switch (command) {
        case 10:
            return cu_read(operand);
        case 11:
            return cu_write(operand);
        default:
            break;
    }
    return EXIT_FAILURE;
}

int cu_read(int operand) {
    if (rk_mytermregime(0, 0, 4, 1, 0)) return EXIT_FAILURE;
    char bf[5] = {0};
    I_printInputField(1, "Read: ");
    if (read(STDIN_FILENO, bf, 4) == -1) return EXIT_FAILURE;
    long long c = xtoll(bf);
    if (c > INT32_MAX || c < INT32_MIN) {
        sc_regSet(err_out_of_range, 1);
        return EXIT_SUCCESS;
    }
    if (!c && bf[0] != '0') {
        return EXIT_FAILURE;
    }
    I_printInputField(0, NULL);
    return sc_memorySet(operand, c);
}

int cu_write(int operand) {
    int c, ret = sc_memoryGet(operand, &c);
    return I_printOutputField("%04X", c) && ret;
}
