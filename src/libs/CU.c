#include <sc/ALU.h>
#include <sc/CU.h>
#include <sc/interface.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

int accumulator = 0;

int CU() {
    int command, operand, val;
    if (sc_memoryGet(instructionCounter, &val)) return EXIT_FAILURE;
    if (sc_commandDecode(val, &command, &operand)) return EXIT_FAILURE;
    switch (command) {
        case 0x10:
            return cu_read(operand);
        case 0x11:
            return cu_write(operand);
        case 0x20:
            return cu_load(operand);
        case 0x21:
            return cu_store(operand);
        case 0x40:
            return cu_jump(operand);
        case 0x41:
            return cu_jneg(operand);
        case 0x42:
            return cu_jz(operand);
        case 0x43:
            return cu_halt();
        case 0x30:
            return alu_add(operand);
        case 0x31:
            return alu_sub(operand);
        case 0x32:
            return alu_divide(operand);
        case 0x33:
            return alu_mul(operand);
        case 0x62:
            return alu_rcl(operand);
        default:
            sc_regSet(err_invalid_command, 1);
            break;
    }
    return EXIT_FAILURE;
}

int cu_read(int operand) {
    if (rk_mytermsave()) return EXIT_FAILURE;
    if (rk_mytermregime(0, 0, 2, 1, 0)) return EXIT_FAILURE;
    char bf1[3] = {0};
    char bf2[3] = {0};
    I_printInputField(1, "Read: ");
    if (read(STDIN_FILENO, bf1, 2) == -1) return EXIT_FAILURE;
    if (read(STDIN_FILENO, bf2, 2) == -1) return EXIT_FAILURE;
    I_printInputField(0, NULL);
    if (rk_mytermrestore()) return EXIT_FAILURE;
    int c1 = xtoll(bf1);
    int c2 = xtoll(bf2);
    if ((!c1 && bf1[0] != '0') || (!c2 && bf2[0] != '0')) return EXIT_FAILURE;
    int val;
    sc_commandEncode(c1, c2, &val);
    return sc_memorySet(operand, val) || I_printhex(operand / 10, operand % 10, color_default, color_default);
}

int cu_write(int operand) {
    int c, ret = sc_memoryGet(operand, &c);
    return I_printOutputField("%04X", c) && ret;
}

int cu_load(int operand) {
    int val;
    if (sc_memoryGet(operand, &val)) return EXIT_FAILURE;
    accumulator = val;
    return EXIT_SUCCESS;
}

int cu_store(int operand) {
    if (sc_memorySet(operand, accumulator)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int cu_jump(int operand) {
    if (operand > 0x63 || operand < 0) {
        sc_regSet(err_out_of_range, 1);
        return EXIT_FAILURE;
    } else {
        instructionCounter = operand;
        return EXIT_SUCCESS;
    }
}

int cu_jneg(int operand) {
    if (operand > 0x63 || operand < 0) {
        sc_regSet(err_out_of_range, 1);
        return EXIT_FAILURE;
    } else if (accumulator < 0) {
        instructionCounter = operand;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int cu_jz(int operand) {
    if (operand > 0x63 || operand < 0) {
        sc_regSet(err_out_of_range, 1);
        return EXIT_FAILURE;
    } else if (!accumulator) {
        instructionCounter = operand;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int cu_halt() { return raise(SIGINT) != -1 ? EXIT_SUCCESS : EXIT_FAILURE; }
