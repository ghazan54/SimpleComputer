#include <fcntl.h>
#include <sc/CU.h>
#include <sc/interface.h>
#include <signal.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <unistd.h>

static int base_x = 2;
static int base_y = 2;

static int instructionCounter = 0;
static int cur_command = 0;

static int cur_x = 0;
static int cur_y = 0;

int I_simplecomputer(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    if (mt_clrscr() || sc_memoryInit() || sc_regInit()) return EXIT_FAILURE;
    if (mt_gotoXY(1, 1)) return EXIT_FAILURE;
    if (bc_box(1, 1, 12, 62) || bc_box(1, 63, 3, 84) || bc_box(4, 63, 6, 84) || bc_box(7, 63, 9, 84) ||
        bc_box(10, 63, 12, 84) || bc_box(13, 1, 22, 46) || bc_box(13, 47, 22, 84) || bc_box(23, 1, 25, 46) ||
        bc_box(23, 47, 25, 84))
        return EXIT_FAILURE;
    // sc_regSet(err_division_by_zero, 1);
    // sc_regSet(err_ignoring_clock_pulses, 1);
    // sc_regSet(err_invalid_command, 1);
    // sc_regSet(err_out_of_range, 1);
    // sc_regSet(err_overflow, 1);
    // for (int i = 0; i < DEFAULT_MEMORY_INIT; ++i) {
    //     sc_memorySet(i, i + 2);
    // }
    I_printall();
    I_startsc();
    return EXIT_SUCCESS;
}

int I_move_address_xy(const int d) {
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
    return EXIT_SUCCESS;
}

void I_stopsc(int sig) {
    sig = sc_memoryFree();
    mt_gotoXY(26, 0);
    exit(EXIT_SUCCESS);
}

int I_startsc(void) {
    rk_mytermregime(0, 0, 1, 0, 1);
    while (1) {
        signal(SIGINT, I_stopsc);
        if (I_printhex(cur_x, cur_y, color_red, color_default)) return EXIT_FAILURE;
        if (I_printinstructionCounter()) return EXIT_FAILURE;
        if (I_printoperations()) return EXIT_FAILURE;
        if (mt_gotoXY(24, 5)) return EXIT_FAILURE;
        enum keys key = 99;
        rk_readkey(&key);
        rk_keyaction(key);
        I_printbig(cur_x, cur_y);
        I_printflags();
    }
    return EXIT_SUCCESS;
}

int I_printall(void) {
    if (I_printinfo('a', color_default, color_default)) return EXIT_FAILURE;
    if (I_printinfo('i', color_default, color_default)) return EXIT_FAILURE;
    if (I_printinfo('o', color_default, color_default)) return EXIT_FAILURE;
    if (I_printinfo('f', color_default, color_default)) return EXIT_FAILURE;
    if (I_printcustomfields()) return EXIT_FAILURE;
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

int createbigchar(int n[2], char* bc) {
    int c = 0;
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (bc_setbigcharpos(n, x, y, bc[c++])) return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int _I_printbig(const char d, int x, int y) {
    int n[2];
    switch (d) {
        case '0':
            createbigchar(n, big_char_0);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '1':
            createbigchar(n, big_char_1);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '2':
            createbigchar(n, big_char_2);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '3':
            createbigchar(n, big_char_3);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '4':
            createbigchar(n, big_char_4);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '5':
            createbigchar(n, big_char_5);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '6':
            createbigchar(n, big_char_6);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '7':
            createbigchar(n, big_char_7);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '8':
            createbigchar(n, big_char_8);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '9':
            createbigchar(n, big_char_9);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '+':
            createbigchar(n, big_char_plus);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case '-':
            createbigchar(n, big_char_minus);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case 'A':
            createbigchar(n, big_char_A);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case 'B':
            createbigchar(n, big_char_B);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case 'C':
            createbigchar(n, big_char_C);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case 'D':
            createbigchar(n, big_char_D);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case 'E':
            createbigchar(n, big_char_E);
            return bc_printbigchar(n, x, y, color_default, color_default);
        case 'F':
            createbigchar(n, big_char_F);
            return bc_printbigchar(n, x, y, color_default, color_default);
        default:
            return EXIT_FAILURE;
    }
}

int I_printbig(int x, int y) {
    int c;
    if (sc_memoryGet(DEFAULT_MAX_STRS * x + y, &c)) return EXIT_FAILURE;
    char digit[5];
    sprintf(digit, "%04X", c);
    char sign = c < 0 ? '-' : '+';
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
    accumulator < 0 ? printf("-") : printf("+");
    printf("%04X", accumulator);
    return EXIT_SUCCESS;
}

int I_printinstructionCounter() {
    if (mt_gotoXY(4, 64)) return EXIT_FAILURE;
    printf(" instructionCounter ");
    if (mt_gotoXY(I_POS_IC_X, I_POS_IC_Y)) return EXIT_FAILURE;
    instructionCounter = DEFAULT_MAX_STRS * cur_x + cur_y;
    printf("+%04X", instructionCounter);
    return EXIT_SUCCESS;
}

int I_printoperations() {
    if (mt_gotoXY(7, 68)) return EXIT_FAILURE;
    printf(" Operation ");
    if (mt_gotoXY(I_POS_OPERATION_X, I_POS_OPERATION_Y)) return EXIT_FAILURE;
    // operations = instructionCounter | (cur_command << 8);
    // sc_commandDecode(operations, &cur_command, &instructionCounter);
    printf("+%02X : %02X", cur_command, instructionCounter);
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

int I_printcustomfields() {
    // bc_box(23, 1, 25, 46) || bc_box(23, 47, 25, 84)
    if (mt_gotoXY(23, 3)) return EXIT_FAILURE;
    printf(" Input ");
    if (mt_gotoXY(24, 2)) return EXIT_FAILURE;
    printf(" > ");
    if (mt_gotoXY(23, 49)) return EXIT_FAILURE;
    printf(" Output ");
    return EXIT_SUCCESS;
}

int I_printInputField(bool status, const char* format, ...) {
    if (mt_gotoXY(24, 2)) return EXIT_FAILURE;
    for (int i = 1; i < 45; ++i) printf(" ");
    if (mt_gotoXY(24, 2)) return EXIT_FAILURE;
    status ? printf(" \e[31m>\e[39m ") : printf(" > ");
    if (format) {
        va_list ap;
        va_start(ap, format);
        vprintf(format, ap);
    }
    return EXIT_SUCCESS;
}

int I_printOutputField(const char* format, ...) {
    if (mt_gotoXY(24, 49)) return EXIT_FAILURE;
    for (int i = 49; i < 84; ++i) printf(" ");
    if (mt_gotoXY(24, 49)) return EXIT_FAILURE;
    if (format) {
        va_list ap;
        va_start(ap, format);
        vprintf(format, ap);
    }
    return EXIT_SUCCESS;
}

int I_executeOperation() {
    if (rk_mytermsave()) return EXIT_FAILURE;
    if (rk_mytermregime(0, 0, 2, 1, 0)) return EXIT_FAILURE;
    char bf[3] = {0};
    int ret = 0;
    I_printInputField(1, NULL);
    if (read(STDIN_FILENO, bf, 2) == -1) return EXIT_FAILURE;
    cur_command = atoi(bf);
    if (sc_commandEncode(cur_command, instructionCounter, &operations)) {
        I_printOutputField("Unknown command '%s'", bf);
        sc_regSet(err_invalid_command, 1);
    } else {
        ret = CU(operations);
    }
    I_printInputField(0, NULL);
    if (rk_mytermrestore()) return EXIT_FAILURE;
    return ret;
}
