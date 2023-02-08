#include <simple_computer.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("%d\n", mt_setfgcolor(color_black));
    printf("%d\n", mt_setbgcolor(color_white));
    sc_memoryInit();
    sc_regInit();
    // // int code, com = 51, oper = 89; // 6617
    // int com, oper;
    // sc_commandDecode(6617, &com, &oper);
    // printf("%d\n", com);
    // printf("%d\n", oper);

    printf("%d\n", sc_memoryLoad("tmpfile.tmp"));
    sc_memoryOutput();
    sc_memoryFree();

    return 0;
}
