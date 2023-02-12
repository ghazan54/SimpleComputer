#include <simple_computer.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // printf("%d\n", mt_setfgcolor(color_green));
    // printf("%d\n", mt_setbgcolor(color_red));
    sc_memoryInit();
    sc_regInit();
    // // int code, com = 51, oper = 89; // 6617
    // int com, oper;
    // sc_commandDecode(6617, &com, &oper);
    // printf("%d\n", com);
    // printf("%d\n", oper);

    // printf("%d\n", sc_memoryLoad("tmpfile.tmp"));
    // sc_memoryOutput();
    sc_memoryFree();

    // bc_box(5, 5, 10, 10);
    int n[2] = {0x8181817E, 0x7E818181};  // 0
    bc_printbigchar(n, 5, 5, color_blue, color_black);

    return 0;
}
