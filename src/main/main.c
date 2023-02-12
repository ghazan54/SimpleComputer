#include <simple_computer.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // printf("cls = %d\n", mt_clrscr());
    // printf("goto = %d\n", mt_gotoXY(10, 10));
    // printf("fg = %d\n", mt_setfgcolor(color_blue));
    // printf("bg = %d\n", mt_setbgcolor(color_black));
    // int h, w;
    // printf("ws = %d\n", mt_getscreensize(&h, &w));
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
