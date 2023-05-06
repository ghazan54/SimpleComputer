#include <sc/ALU.h>
#include <sc/CU.h>
#include <sc/helper.h>
#include <sc/interface.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <unistd.h>

int accumulator = 0;

int CU() {
    int command, operand, val;
    if (sc_memoryGet(instructionCounter, &val)) return ERROR_CODE;
    if (sc_commandDecode(val, &command, &operand)) return ERROR_CODE;
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
            sc_regSet(err_ignoring_clock_pulses, 1);
            I_printflags();
            while (1) {
                usleep(100000);
                I_printInputField(0, NULL);
            }
            break;
    }
    return ERROR_CODE;
}

int cu_read(int operand) {
    int c1, val;
    I_printInputField(1, " Read: ");
    scanf("%04X", &c1);
    __fpurge(stdin);
    I_printInputField(0, NULL);
    I_printInputField(0, NULL);
    if (c1 < 0)
        val = ((-c1) & 0x3fff) | 0x4000;
    else
        val = c1 & 0x3fff;
    return sc_memorySet(operand, val) || I_printhex(operand, color_default, color_default);
}

int cu_write(int operand) {
    int c, ret = sc_memoryGet(operand, &c);
    return I_printOutputField("%c%X", c & 0x4000 ? '-' : '+', c & 0x3fff) || ret;
}

int cu_load(int operand) {
    int val;
    if (sc_memoryGet(operand, &val)) return ERROR_CODE;
    accumulator = val;
    return SUCCES_CODE;
}

int cu_store(int operand) {
    if (sc_memorySet(operand, accumulator) || I_printhex(operand, color_default, color_default))
        return ERROR_CODE;
    return SUCCES_CODE;
}

int cu_jump(int operand) {
    if (operand > 0x63 || operand < 0) {
        sc_regSet(err_out_of_range, 1);
        return ERROR_CODE;
    } else {
        last_jump = true;
        I_moveInstructionCounter(operand);
        return SUCCES_CODE;
    }
}

int cu_jneg(int operand) {
    if (operand > 0x63 || operand < 0) {
        sc_regSet(err_out_of_range, 1);
        return ERROR_CODE;
    } else if (accumulator & 0x4000) {
        last_jump = true;
        I_moveInstructionCounter(operand);
        return SUCCES_CODE;
    }
    return ERROR_CODE;
}

int cu_jz(int operand) {
    if (operand > 0x63 || operand < 0) {
        sc_regSet(err_out_of_range, 1);
        return ERROR_CODE;
    } else if (!accumulator) {
        last_jump = true;
        I_moveInstructionCounter(operand);
        return SUCCES_CODE;
    }
    return ERROR_CODE;
}

int cu_halt() { return I_stopprogram(); }
