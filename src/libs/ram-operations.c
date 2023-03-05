#include <sc/ram-operations.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int* memory = NULL;
static int memory_flags;
static int memory_operations_id[DEFAULT_COUNT_MEMORY_OPERATIONS] = {
    10, 11,          // i/o
    20, 21,          // loading/unloading
    30, 31, 32, 33,  // arithmetic
    40, 41, 42, 43,  // control transfer
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76};  // custom

int sc_memoryInit(void) {
    memory = (int*)calloc(DEFAULT_MEMORY_INIT, sizeof(int));
    if (memory) {
        memset(memory, 0, DEFAULT_MEMORY_INIT * sizeof(int));
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
    // return memory ? EXIT_SUCCESS : EXIT_FAILURE;
}

int sc_memorySet(int address, int value) {
    if (address < 0 || address >= DEFAULT_MEMORY_INIT) {
        sc_regSet(err_out_of_range, 1);
        return EXIT_FAILURE;
    } else {
        memory[address] = value;
        return EXIT_SUCCESS;
    }
}

int sc_memoryGet(int address, int* value) {
    if (address < 0 || address >= DEFAULT_MEMORY_INIT) {
        sc_regSet(err_out_of_range, 1);
        return EXIT_FAILURE;
    } else {
        *value = memory[address];
        return EXIT_SUCCESS;
    }
}

int sc_memorySave(char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) return EXIT_FAILURE;
    int r = fwrite(&memory[0], sizeof(int) * DEFAULT_MEMORY_INIT, 1, f);
    fclose(f);
    return !r;
}

int sc_memoryLoad(char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return EXIT_FAILURE;
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
        return EXIT_FAILURE;
    } else {
        memory_flags = value ? memory_flags | (MASK_LOW_BIT << (registr - 1))
                             : memory_flags & (~(MASK_LOW_BIT << (registr - 1)));
        return EXIT_SUCCESS;
    }
}

int sc_regGet(int registr, int* value) {
    if (registr < 1 || registr > DEFAULT_FLAGS_COUNT) {
        return EXIT_FAILURE;
    } else {
        *value = (memory_flags >> (registr - 1)) & MASK_LOW_BIT;
        return EXIT_SUCCESS;
    }
}

int cmpint_bs(const void* a, const void* b) { return (*(int*)a - *(int*)b); }

int sc_commandEncode(int command, int operand, int* value) {
    if (!bsearch(&command, memory_operations_id, DEFAULT_COUNT_MEMORY_OPERATIONS, sizeof(int), cmpint_bs)) {
        sc_regSet(err_invalid_command, 1);
        return EXIT_FAILURE;
    }
    *value = (((*value & 0) | operand) & MASK_OPERAND_BITS) |
             ((command << DEFAULT_BLOCK_ENCODE_BITS) & MASK_COMMAND_BITS);
    return EXIT_SUCCESS;
}

int sc_commandDecode(int value, int* command, int* operand) {
    if (((value & 0x4000) >> 14) & MASK_LOW_BIT)  // 0x4000 == 0x100000000000000
        return EXIT_FAILURE;
    *operand = value & MASK_OPERAND_BITS;
    *command = (value & MASK_COMMAND_BITS) >> DEFAULT_BLOCK_ENCODE_BITS;
    return EXIT_SUCCESS;
}

void sc_memoryOutput(void) {
    for (int i = 0; i < DEFAULT_MEMORY_INIT; ++i) printf("%d ", memory[i]);
    printf("\n");
}

void sc_memoryAddressOutput(int x, int y) {
    if (x < 10 && y < 10) {
        int val = memory[DEFAULT_MAX_STRS * x + y] & 0xFFFF;
        memory[DEFAULT_MAX_STRS * x + y] < 0 ? printf("-") : printf("+");
        printf("%04X", val);
    }
}

int sc_memoryFree(void) {
    if (memory) {
        free(memory);
        memory = NULL;
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
