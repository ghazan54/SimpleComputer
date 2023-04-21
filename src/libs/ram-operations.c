#include <sc/helper.h>
#include <sc/ram-operations.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int* memory = NULL;
static int memory_flags;
static int memory_operations_id[DEFAULT_COUNT_MEMORY_OPERATIONS] = {
    0x10, 0x11,              // i/o
    0x20, 0x21,              // loading/unloading
    0x30, 0x31, 0x32, 0x33,  // arithmetic
    0x40, 0x41, 0x42, 0x43,  // control transfer
    0x62};                   // custom

int sc_memoryInit(void) {
    memory = (int*)calloc(DEFAULT_MEMORY_INIT, sizeof(int));
    if (memory) {
        memset(memory, 0, DEFAULT_MEMORY_INIT * sizeof(int));
        return SUCCES_CODE;
    } else {
        return ERROR_CODE;
    }
}

int sc_memorySet(int address, int value) {
    if (address < 0 || address >= DEFAULT_MEMORY_INIT) {
        sc_regSet(err_out_of_range, 1);
        return ERROR_CODE;
    } else {
        // if (value > 0x3fff) {
        //     sc_regSet(err_out_of_range, 1);
        //     return ERROR_CODE;
        // }
        memory[address] = value;
        return SUCCES_CODE;
    }
}

int sc_memoryGet(int address, int* value) {
    if (address < 0 || address >= DEFAULT_MEMORY_INIT) {
        sc_regSet(err_out_of_range, 1);
        return ERROR_CODE;
    } else {
        *value = memory[address];
        return SUCCES_CODE;
    }
}

int sc_memorySave(char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) return ERROR_CODE;
    int r = fwrite(&memory[0], sizeof(int) * DEFAULT_MEMORY_INIT, 1, f);
    fclose(f);
    return !r;
}

int sc_memoryLoad(char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return ERROR_CODE;
    int r = fread(&memory[0], sizeof(int) * DEFAULT_MEMORY_INIT, 1, f);
    fclose(f);
    return !r;
}

int sc_regInit(void) {
    memory_flags = 0;
    return memory_flags;
}

int sc_regSet(int registr, int value) {
    if (registr < 1 || registr > DEFAULT_FLAGS_COUNT) {
        return ERROR_CODE;
    } else {
        memory_flags = value ? memory_flags | (MASK_LOW_BIT << (registr - 1))
                             : memory_flags & (~(MASK_LOW_BIT << (registr - 1)));
        return SUCCES_CODE;
    }
}

int sc_regGet(int registr, int* value) {
    if (registr < 1 || registr > DEFAULT_FLAGS_COUNT) {
        return ERROR_CODE;
    } else {
        *value = (memory_flags >> (registr - 1)) & MASK_LOW_BIT;
        return SUCCES_CODE;
    }
}

int cmpint_bs(const void* a, const void* b) { return (*(int*)a - *(int*)b); }

int sc_commandEncode(int command, int operand, int* value) {
    if (!bsearch(&command, memory_operations_id, DEFAULT_COUNT_MEMORY_OPERATIONS, sizeof(int), cmpint_bs)) {
        return ERROR_CODE;
    } else {
        *value = 0;
    }
    *value |= command << DEFAULT_BLOCK_ENCODE_BITS;
    *value |= operand;
    return SUCCES_CODE;
}

int sc_commandDecode(int value, int* command, int* operand) {
    // if (((value & 0x4000) >> 14) & MASK_LOW_BIT) // 0x4000 == 0x100000000000000
    //     return ERROR_CODE;
    *operand = value & MASK_OPERAND_BITS;
    *command = (value >> DEFAULT_BLOCK_ENCODE_BITS) & MASK_OPERAND_BITS;
    return SUCCES_CODE;
}

void sc_memoryOutput(void) {
    for (int i = 0; i < DEFAULT_MEMORY_INIT; ++i) sc_print("%d ", memory[i]);
    sc_print("\n");
}

void sc_memoryAddressOutput(int x, int y) {
    if (x < 10 && y < 10 && x >= 0 && y >= 0) {
        int val, command, operand;
        if (sc_memoryGet(DEFAULT_MAX_STRS * x + y, &val) ||
            sc_commandDecode(val & 0x3FFF, &command, &operand))
            return;
        sc_print("%c%02X%02X", val & 0x4000 ? '-' : '+', command, operand);
    }
}

int sc_memoryFree(void) {
    if (memory) {
        free(memory);
        memory = NULL;
        return SUCCES_CODE;
    } else {
        return ERROR_CODE;
    }
}
