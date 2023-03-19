#include <sc/ALU.h>
#include <sc/CU.h>
#include <sc/ram-operations.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

int alu_add(int operand) {
    int val;
    if (sc_memoryGet(operand, &val)) return EXIT_FAILURE;
    long long tmp = (long long)accumulator + (long long)val;
    if (tmp > 0x7f7f || tmp < 0x7f7f) {
        sc_regSet(err_out_of_range, 1);
        return EXIT_FAILURE;
    }
    accumulator += val;
    return EXIT_SUCCESS;
}

int alu_sub(int operand) {
    int val;
    if (sc_memoryGet(operand, &val)) return EXIT_FAILURE;
    long long tmp = (long long)accumulator - (long long)val;
    if (tmp > 0x7f7f || tmp < 0x7f7f) {
        sc_regSet(err_out_of_range, 1);
        return EXIT_FAILURE;
    }
    accumulator -= val;
    return EXIT_SUCCESS;
}

int alu_divide(int operand) {
    int val;
    if (sc_memoryGet(operand, &val)) return EXIT_FAILURE;
    if (!val) {
        sc_regSet(err_division_by_zero, 1);
        return EXIT_FAILURE;
    }
    accumulator /= val;
    return EXIT_SUCCESS;
}

int alu_mul(int operand) {
    int val;
    if (sc_memoryGet(operand, &val)) return EXIT_FAILURE;
    long long tmp = (long long)accumulator * (long long)val;
    if (tmp > 0x7f7f || tmp < 0x7f7f) {
        sc_regSet(err_out_of_range, 1);
        return EXIT_FAILURE;
    }
    accumulator *= val;
    return EXIT_SUCCESS;
}
