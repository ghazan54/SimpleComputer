#include <ctest.h>
#include <simple_computer.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

CTEST(ram_operations, sc_memoryInit) {
    ASSERT_EQUAL(DEFAULT_MEMORY_INIT, 100);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_memoryInit());
    for (int i = 0; i < DEFAULT_MEMORY_INIT; ++i) {
        // ASSERT_EQUAL(0, memory[i]);
        int v;
        ASSERT_EQUAL(EXIT_SUCCESS, sc_memoryGet(i, &v));
        ASSERT_EQUAL(0, v);
    }
    ASSERT_EQUAL(EXIT_SUCCESS, sc_memoryFree());
}

CTEST(ram_operations, sc_memorySet) {
    sc_memoryInit();
    int v;

    ASSERT_EQUAL(EXIT_SUCCESS, sc_memorySet(0, __INT32_MAX__));
    ASSERT_EQUAL(EXIT_SUCCESS, sc_memoryGet(0, &v));
    ASSERT_EQUAL(__INT32_MAX__, v);
    // ASSERT_EQUAL(__INT32_MAX__, memory[0]);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_memorySet(0, 3));
    // ASSERT_EQUAL(3, memory[0]);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_memoryGet(0, &v));
    ASSERT_EQUAL(3, v);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_memorySet(10, 5));
    // ASSERT_EQUAL(5, memory[10]);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_memoryGet(10, &v));
    ASSERT_EQUAL(5, v);
    ASSERT_EQUAL(EXIT_FAILURE, sc_memorySet(-1, 3));
    ASSERT_EQUAL(EXIT_FAILURE, sc_memorySet(100, 3));

    ASSERT_EQUAL(EXIT_SUCCESS, sc_memoryFree());
}

CTEST(ram_operations, sc_memoryGet) {
    sc_memoryInit();
    for (int i = 0; i < DEFAULT_MEMORY_INIT; ++i) {
        ASSERT_EQUAL(EXIT_SUCCESS, sc_memorySet(i, i));
        // memory[i] = i;
    }
    for (int i = 0; i < DEFAULT_MEMORY_INIT; ++i) {
        int v;
        ASSERT_EQUAL(EXIT_SUCCESS, sc_memoryGet(i, &v));
        ASSERT_EQUAL(i, v);
    }
    ASSERT_EQUAL(EXIT_FAILURE, sc_memoryGet(-1, NULL));
    ASSERT_EQUAL(EXIT_FAILURE, sc_memoryGet(100, NULL));

    ASSERT_EQUAL(EXIT_SUCCESS, sc_memoryFree());
}

CTEST(ram_operations, sc_memorySave) {
    char tmpnamefile[L_tmpnam] = "tmpfile.tmp\0";
    sc_memoryInit();
    for (int i = 0; i < DEFAULT_MEMORY_INIT; ++i) {
        ASSERT_EQUAL(EXIT_SUCCESS, sc_memorySet(i, i));
    }
    ASSERT_EQUAL(EXIT_SUCCESS, sc_memorySave(tmpnamefile));
    ASSERT_EQUAL(EXIT_FAILURE, sc_memorySave(NULL));
    remove("tmpfile.tmp\0");
    ASSERT_EQUAL(EXIT_SUCCESS, sc_memoryFree());
}

CTEST(ram_operations, sc_memoryLoad) {
    char tmpnamefile[L_tmpnam] = "tmpfile.tmp\0";
    sc_memoryInit();
    for (int i = 0; i < DEFAULT_MEMORY_INIT; ++i) {
        ASSERT_EQUAL(EXIT_SUCCESS, sc_memorySet(i, i));
    }
    ASSERT_EQUAL(EXIT_SUCCESS, sc_memorySave(tmpnamefile));
    sc_memoryInit();
    ASSERT_EQUAL(EXIT_FAILURE, sc_memoryLoad(NULL));
    ASSERT_EQUAL(EXIT_SUCCESS, sc_memoryLoad(tmpnamefile));
    for (int i = 0; i < DEFAULT_MEMORY_INIT; ++i) {
        // ASSERT_EQUAL(i, memory[i]);
        int v;
        ASSERT_EQUAL(EXIT_SUCCESS, sc_memoryGet(i, &v));
        ASSERT_EQUAL(i, v);
    }
    remove("tmpfile.tmp\0");
    ASSERT_EQUAL(EXIT_SUCCESS, sc_memoryFree());
}

CTEST(ram_operations, sc_regSet) {
    sc_regInit();
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regSet(1, 1));
    int v;
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regGet(1, &v));
    ASSERT_EQUAL(1, v);
    // ASSERT_EQUAL(MASK_LOW_BIT, memory_flags & MASK_LOW_BIT);
    ASSERT_EQUAL(EXIT_FAILURE, sc_regSet(0, 1));
    ASSERT_EQUAL(EXIT_FAILURE, sc_regSet(6, 1));
    sc_regInit();  // memory_flags = 0;
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regSet(5, 1));
    // ASSERT_EQUAL(MASK_LOW_BIT << 4, memory_flags);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regGet(5, &v));
    ASSERT_EQUAL(1, v);
}

CTEST(ram_operations, sc_regGet) {
    sc_regInit();
    // memory_flags |= 0x1f; // 0x11111
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regSet(1, 1));
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regSet(2, 1));
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regSet(3, 1));
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regSet(4, 1));
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regSet(5, 1));
    int v;
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regGet(1, &v));
    ASSERT_EQUAL(1, v);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regGet(2, &v));
    ASSERT_EQUAL(1, v);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regGet(3, &v));
    ASSERT_EQUAL(1, v);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regGet(4, &v));
    ASSERT_EQUAL(1, v);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regGet(5, &v));
    ASSERT_EQUAL(1, v);
    sc_regInit();  // memory_flags &= 0;
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regGet(1, &v));
    ASSERT_EQUAL(0, v);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regGet(2, &v));
    ASSERT_EQUAL(0, v);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regGet(3, &v));
    ASSERT_EQUAL(0, v);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regGet(4, &v));
    ASSERT_EQUAL(0, v);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_regGet(5, &v));
    ASSERT_EQUAL(0, v);

    ASSERT_EQUAL(EXIT_FAILURE, sc_regGet(6, &v));
    ASSERT_EQUAL(EXIT_FAILURE, sc_regGet(0, &v));
}

CTEST(ram_operations, sc_commandEncode) {
    int v;
    ASSERT_EQUAL(EXIT_SUCCESS, sc_commandEncode(32, 1, &v));
    ASSERT_EQUAL(0x1001, v);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_commandEncode(51, 89, &v));
    ASSERT_EQUAL(0x19d9, v);
    ASSERT_EQUAL(EXIT_FAILURE, sc_commandEncode(1, 1, &v));
    ASSERT_EQUAL(EXIT_FAILURE, sc_commandEncode(15, 1, &v));
}

CTEST(ram_operations, sc_commandDecode) {
    int c, o;
    ASSERT_EQUAL(EXIT_SUCCESS, sc_commandDecode(0x1001, &c, &o));
    ASSERT_EQUAL(32, c);
    ASSERT_EQUAL(1, o);
    ASSERT_EQUAL(EXIT_SUCCESS, sc_commandDecode(0x19d9, &c, &o));
    ASSERT_EQUAL(51, c);
    ASSERT_EQUAL(89, o);
    ASSERT_EQUAL(EXIT_FAILURE, sc_commandDecode(0x4000, &c, &o));
}
