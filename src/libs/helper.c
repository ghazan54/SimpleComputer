#include <ctype.h>
#include <math.h>
#include <sc/helper.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int xtoll(char* s) {
    int i, sum = 0, k;
    int p = (int)strlen(s) - 1;
    for (i = 0; s[i] != '\0'; i++) {
        switch (toupper(s[i])) {
            case 'A':
                k = 10;
                break;
            case 'B':
                k = 11;
                break;
            case 'C':
                k = 12;
                break;
            case 'D':
                k = 13;
                break;
            case 'E':
                k = 14;
                break;
            case 'F':
                k = 15;
                break;
            case '1':
                k = 1;
                break;
            case '2':
                k = 2;
                break;
            case '3':
                k = 3;
                break;
            case '4':
                k = 4;
                break;
            case '5':
                k = 5;
                break;
            case '6':
                k = 6;
                break;
            case '7':
                k = 7;
                break;
            case '8':
                k = 8;
                break;
            case '9':
                k = 9;
                break;
            case '0':
                k = 0;
                break;
            default:
                return 0;
        }
        sum += k * pow(16, p--);
    }
    return sum;
}

int sc_print(const char* format, ...) {
    if (format) {
        va_list ap;
        va_start(ap, format);
        char buf[BUFSIZ];
        vsprintf(buf, format, ap);
        write(STDOUT_FILENO, buf, strlen(buf));
    }
    return SUCCES_CODE;
}
