#include <sc/ALU.h>
#include <sc/CU.h>
#include <sc/interface.h>
#include <sc/ram-operations.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int alu_add(int operand) {
    int val;
    if (sc_memoryGet(operand, &val)) return EXIT_FAILURE;
    bool sign = accumulator & 0x4000 ? true : false;
    accumulator &= 0x3fff;
    accumulator = sign ? -accumulator : accumulator;
    long long tmp = (long long)accumulator + ((long long)val & 0x3fff);
    if (tmp > 0x3fff || tmp < -0x3fff)
        accumulator = 0;
    else
        accumulator += val & 0x3fff;

    if (accumulator < 0) {
        accumulator = abs(accumulator);
        accumulator |= 0x4000;
    } else {
        accumulator = abs(accumulator);
        accumulator &= 0x3fff;
    }
    return I_printaccumulator();
}

int alu_sub(int operand) {
    int val;
    if (sc_memoryGet(operand, &val)) return EXIT_FAILURE;
    bool sign = accumulator & 0x4000 ? true : false;
    accumulator &= 0x3fff;
    accumulator = sign ? -accumulator : accumulator;
    long long tmp = (long long)accumulator - ((long long)val & 0x3fff);
    if (tmp > 0x3fff || tmp < -0x3fff)
        accumulator = 0;
    else
        accumulator -= val & 0x3fff;

    if (accumulator < 0) {
        accumulator = abs(accumulator);
        accumulator |= 0x4000;
    } else {
        accumulator = abs(accumulator);
        accumulator &= 0x3fff;
    }
    return I_printaccumulator();
}

int alu_divide(int operand) {
    int val;
    if (sc_memoryGet(operand, &val)) return EXIT_FAILURE;
    if (!(val & 0x3fff)) {
        sc_regSet(err_division_by_zero, 1);
        return EXIT_FAILURE;
    }
    bool sign = accumulator & 0x4000 ? true : false;
    accumulator &= 0x3fff;
    accumulator = sign ? -accumulator : accumulator;

    accumulator /= val & 0x3fff;

    if (accumulator < 0) {
        accumulator = abs(accumulator);
        accumulator |= 0x4000;
    } else {
        accumulator = abs(accumulator);
        accumulator &= 0x3fff;
    }
    return I_printaccumulator();
}

int alu_mul(int operand) {
    int val;
    if (sc_memoryGet(operand, &val)) return EXIT_FAILURE;
    bool sign = accumulator & 0x4000 ? true : false;
    accumulator &= 0x3fff;
    accumulator = sign ? -accumulator : accumulator;
    long long tmp = (long long)accumulator * ((long long)val & 0x3fff);
    if (tmp > 0x3fff || tmp < -0x3fff)
        accumulator = 0;
    else
        accumulator *= val & 0x3fff;

    if (accumulator < 0) {
        accumulator = abs(accumulator);
        accumulator |= 0x4000;
    } else {
        accumulator = abs(accumulator);
        accumulator &= 0x3fff;
    }
    return I_printaccumulator();
}

int alu_rcl(int operand) {
    int val, v1, v2;
    if (sc_memoryGet(operand, &val) || sc_commandDecode(val, &v1, &v2)) return EXIT_FAILURE;
    char bf[5] = {0};
    sprintf(bf, "%02X%02X", v1, v2);
    val = xtoll(bf);
    int msb = (val >> 13) & 1;
    val <<= 1;
    val |= msb;
    val &= 0x3fff;
    accumulator = val;
    return I_printaccumulator();
}