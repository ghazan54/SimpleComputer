#include <fcntl.h>
#include <sc/interface.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

static int base_x = 2;
static int base_y = 2;

static int accumulator = 0;
static int instructionCounter = 0;
static int operations = 0;

static int cur_x = 0;
static int cur_y = 0;

static bool sc_continue = true;

int I_simplecomputer(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    if (mt_clrscr() || sc_memoryInit() || sc_regInit()) return EXIT_FAILURE;
    if (mt_gotoXY(1, 1)) return EXIT_FAILURE;
    if (bc_box(1, 1, 12, 62) || bc_box(1, 63, 3, 84) || bc_box(4, 63, 6, 84) || bc_box(7, 63, 9, 84) ||
        bc_box(10, 63, 12, 84) || bc_box(13, 1, 22, 46) || bc_box(13, 47, 22, 84))
        return EXIT_FAILURE;
    sc_memorySet(7, 0x2 | 0x4000);
    sc_regSet(err_division_by_zero, 1);
    sc_regSet(err_ignoring_clock_pulses, 1);
    sc_regSet(err_invalid_command, 1);
    sc_regSet(err_out_of_range, 1);
    sc_regSet(err_overflow, 1);
    I_printall();
    // I_printhex(0, 7, color_blue, color_red);
    // mt_clrscr();
    I_startsc();
    return EXIT_SUCCESS;
}

int move_address_xy(const int d) {
    if (I_printhex(cur_x, cur_y, color_default, color_default)) return EXIT_FAILURE;
    switch (d) {
        case 0:
            if (!cur_x)
                cur_x = DEFAULT_MAX_STRS - 1;
            else
                cur_x -= 1;
            break;
        case 1:
            if (cur_x == DEFAULT_MAX_STRS - 1)
                cur_x = 0;
            else
                cur_x += 1;
            break;
        case 2:
            if (cur_y == DEFAULT_MAX_STRS - 1)
                cur_y = 0;
            else
                cur_y += 1;
            break;
        case 3:
            if (!cur_y)
                cur_y = DEFAULT_MAX_STRS - 1;
            else
                cur_y -= 1;

            break;

        default:
            break;
    }
}

int I_startsc(void) {
    int cmd = -1;
    while (1) {
        if (I_printhex(cur_x, cur_y, color_red, color_default)) return EXIT_FAILURE;
        if (I_printinstructionCounter()) return EXIT_FAILURE;
        if (mt_gotoXY(23, 0)) return EXIT_FAILURE;
        char c = 0;
        if (read(1, &c, 1) != 1) exit(EXIT_FAILURE);
        switch (c) {
            case '\033':
                cmd = 1;
                break;
            case 'l':
                cmd = 0;
                printf("Press L\n");
                break;
            case '[':
                if (cmd == 1) {
                    cmd = 2;
                } else {
                    printf("Unknown key!!\n");
                }
                break;
            case '1':
                if (cmd == 2) {
                    cmd = 3;
                } else {
                    printf("Unknown key!!\n");
                }
                break;
            case '5':
                if (cmd == 3) {
                    cmd = 4;
                } else {
                    printf("Unknown key!!\n");
                }
                break;
            case '7':
                if (cmd == 3) {
                    cmd = 5;
                } else {
                    printf("Unknown key!!\n");
                }
                break;
            case '~':
                if (cmd == 4) {
                    printf("Pressed F5!\n");
                    cmd = 0;
                } else if (cmd == 5) {
                    printf("Pressed F6!\n");
                    cmd = 0;
                } else {
                    printf("Unknown key!!\n");
                }
                break;
            case 'A':
                if (cmd == 2) {
                    move_address_xy(0);
                    cmd = 0;
                } else {
                    printf("Unknown key!!\n");
                }
                break;
            case 'B':
                if (cmd == 2) {
                    move_address_xy(1);
                    cmd = 0;
                } else {
                    printf("Unknown key!!\n");
                }
                break;
            case 'C':
                if (cmd == 2) {
                    move_address_xy(2);
                    cmd = 0;
                } else {
                    printf("Unknown key!!\n");
                }
                break;
            case 'D':
                if (cmd == 2) {
                    move_address_xy(3);
                    cmd = 0;
                } else {
                    printf("Unknown key!!\n");
                }
                break;
        }
    }
    return EXIT_SUCCESS;
}

int I_printall(void) {
    if (I_printinfo('a', color_default, color_default)) return EXIT_FAILURE;
    if (I_printinfo('i', color_default, color_default)) return EXIT_FAILURE;
    if (I_printinfo('o', color_default, color_default)) return EXIT_FAILURE;
    if (I_printinfo('f', color_default, color_default)) return EXIT_FAILURE;

    if (mt_gotoXY(1, 29)) return EXIT_FAILURE;
    printf(" Memory ");
    for (int i = 0; i < DEFAULT_MAX_STRS; ++i) {
        for (int j = 0; j < DEFAULT_MAX_STRS; ++j) {
            if (I_printhex(i, j, color_default, color_default)) return EXIT_FAILURE;
        }
    }
    if (I_printbig(0, 0)) return EXIT_FAILURE;
    if (I_printkeys()) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int I_printhex(int i, int j, enum colors fg, enum colors bg) {
    if (mt_setfgcolor(fg)) return EXIT_FAILURE;
    if (mt_setbgcolor(bg)) return EXIT_FAILURE;
    if (i >= DEFAULT_MAX_STRS || j >= DEFAULT_MAX_STRS || i < 0 || j < 0) {
        return EXIT_FAILURE;
    } else {
        if (mt_gotoXY(i + base_x, j ? j * 6 + base_y : base_y)) return EXIT_FAILURE;
        sc_memoryAddressOutput(i, j);
    }
    if (mt_setfgcolor(color_default)) return EXIT_FAILURE;
    if (mt_setbgcolor(color_default)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int I_printinfo(const char I, enum colors fg, enum colors bg) {
    int c = 1;
    if (mt_setfgcolor(fg)) return EXIT_FAILURE;
    if (mt_setbgcolor(bg)) return EXIT_FAILURE;
    switch (I) {
        case 'a':
            c = I_printaccumulator();
            break;
        case 'i':
            c = I_printinstructionCounter();
            break;
        case 'o':
            c = I_printoperations();
            break;
        case 'f':
            c = I_printflags();
            break;
        default:
            c = EXIT_FAILURE;
            break;
    }
    if (mt_setfgcolor(color_default)) return EXIT_FAILURE;
    if (mt_setbgcolor(color_default)) return EXIT_FAILURE;
    return c;
}

int _I_printbig(const char d, int x, int y) {
    int n[2];
    switch (d) {
        case '0':
            n[0] = BC_NIL_1;
            n[1] = BC_NIL_2;
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '1':
            n[0] = BC_ONE_1;
            n[1] = BC_ONE_2;
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '2':
            n[0] = BC_TWO_1;
            n[1] = BC_TWO_2;
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '3':
            n[0] = BC_THREE_1;
            n[1] = BC_THREE_2;
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '4':
            n[0] = BC_FOUR_1;
            n[1] = BC_FOUR_2;
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '5':
            n[0] = BC_FIVE_1;
            n[1] = BC_FIVE_2;
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '6':
            n[0] = BC_SIX_1;
            n[1] = BC_SIX_2;
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '7':
            n[0] = BC_SEVEN_1;
            n[1] = BC_SEVEN_2;
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '8':
            n[0] = BC_EIGHT_1;
            n[1] = BC_EIGHT_2;
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '9':
            n[0] = BC_NINE_1;
            n[1] = BC_NINE_2;
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '+':
            n[0] = BC_PLUS_1;
            n[1] = BC_PLUS_2;
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '-':
            n[0] = BC_MINUS_1;
            n[1] = BC_MINUS_2;
            return bc_printbigchar(n, x, y, color_default, color_default);
        default:
            return EXIT_FAILURE;
    }
}

int I_printbig(int x, int y) {
    int c;
    if (sc_memoryGet(DEFAULT_MAX_STRS * x + y, &c)) return EXIT_FAILURE;
    char digit[5];
    sprintf(digit, "%04X", c & 0x3FFF);
    char sign = c & 0x4000 ? '-' : '+';
    if (_I_printbig(sign, 14, 2) || _I_printbig(digit[0], 14, 11) || _I_printbig(digit[1], 14, 20) ||
        _I_printbig(digit[2], 14, 29) || _I_printbig(digit[3], 14, 38))
        EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int I_printkeys(void) {
    if (mt_gotoXY(13, 48)) return EXIT_FAILURE;
    printf(" Keys ");
    if (mt_gotoXY(14, 48)) return EXIT_FAILURE;
    printf("l - load\n");
    if (mt_gotoXY(15, 48)) return EXIT_FAILURE;
    printf("s - save\n");
    if (mt_gotoXY(16, 48)) return EXIT_FAILURE;
    printf("r - run\n");
    if (mt_gotoXY(17, 48)) return EXIT_FAILURE;
    printf("t - step\n");
    if (mt_gotoXY(18, 48)) return EXIT_FAILURE;
    printf("F5 - accumulator\n");
    if (mt_gotoXY(19, 48)) return EXIT_FAILURE;
    printf("F6 - instructionCounter\n");
    return EXIT_SUCCESS;
}

int I_printaccumulator() {
    if (mt_gotoXY(1, 67)) return EXIT_FAILURE;
    printf(" accumulator ");
    if (mt_gotoXY(I_POS_ACCUMULATOR_X, I_POS_ACCUMULATOR_Y)) return EXIT_FAILURE;
    accumulator & 0x4000 ? printf("-") : printf("+");
    printf("%04X", accumulator & 0x3FFF);
    return EXIT_SUCCESS;
}

int I_printinstructionCounter() {
    if (mt_gotoXY(4, 64)) return EXIT_FAILURE;
    printf(" instructionCounter ");
    if (mt_gotoXY(I_POS_IC_X, I_POS_IC_Y)) return EXIT_FAILURE;
    printf("+");
    printf("%04X", DEFAULT_MAX_STRS * cur_x + cur_y);
    return EXIT_SUCCESS;
}

int I_printoperations() {
    if (mt_gotoXY(7, 68)) return EXIT_FAILURE;
    printf(" Operation ");
    if (mt_gotoXY(I_POS_OPERATION_X, I_POS_OPERATION_Y)) return EXIT_FAILURE;
    operations & 0x4000 ? printf("-") : printf("+");
    printf("%04X", operations & 0x3FFF);
    return EXIT_SUCCESS;
}

int I_printflags() {
    if (mt_gotoXY(10, 70)) return EXIT_FAILURE;
    printf(" Flags ");
    if (mt_gotoXY(I_POS_FLAGS_X, I_POS_FLAGS_Y)) return EXIT_FAILURE;
    int r = 0, c;
    r |= sc_regGet(err_overflow, &c);
    c ? printf("O ") : printf(" ");
    r |= sc_regGet(err_division_by_zero, &c);
    c ? printf("D ") : printf(" ");
    r |= sc_regGet(err_out_of_range, &c);
    c ? printf("R ") : printf(" ");
    r |= sc_regGet(err_ignoring_clock_pulses, &c);
    c ? printf("I ") : printf(" ");
    r |= sc_regGet(err_invalid_command, &c);
    c ? printf("C ") : printf(" ");
    return r;
}
