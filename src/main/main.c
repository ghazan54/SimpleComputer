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
    mt_clrscr();
    // bc_box(5, 5, 14, 14);
    bc_box(5, 5, 24, 14);
    int n[2] = {BC_ONE_1, 0};
    bc_setbigcharpos(n, 4, 6, 1);
    bc_setbigcharpos(n, 5, 6, 1);
    bc_setbigcharpos(n, 6, 6, 1);
    bc_setbigcharpos(n, 7, 6, 1);
    bc_printbigchar(n, 6, 6, color_white, color_black);
    // bc_box(15, 5, 24, 14);
    int n2[2] = {BC_SIX_1, BC_SIX_2};
    bc_printbigchar(n2, 16, 6, color_white, color_black);

    return 0;
}
