#pragma once

#define DEFAULT_MEMORY_INIT (100)
#define DEFAULT_FLAGS_COUNT (5)
#define DEFAULT_COUNT_MEMORY_OPERATIONS (38)
#define DEFAULT_BLOCK_ENCODE_BITS (7)
#define DEFAULT_MAX_STRS (10)

#define MASK_LOW_BIT (0x1)
#define MASK_OPERAND_BITS (0x7f)
#define MASK_COMMAND_BITS (MASK_OPERAND_BITS << DEFAULT_BLOCK_ENCODE_BITS)

enum error_bits_memory {
    err_overflow = 1,
    err_division_by_zero,
    err_out_of_range,
    err_ignoring_clock_pulses,
    err_invalid_command
};

// extern int memory[DEFAULT_MEMORY_INIT];
// extern int memory_flags;
// extern int memory_operations_id[DEFAULT_COUNT_MEMORY_OPERATIONS];

int sc_memoryInit(void);
int sc_memorySet(int address, int value);
int sc_memoryGet(int address, int* value);
int sc_memorySave(char* filename);
int sc_memoryLoad(char* filename);
int sc_regInit(void);
int sc_regSet(int registr, int value);
int sc_regGet(int registr, int* value);
int sc_commandEncode(int command, int operand, int* value);
int sc_commandDecode(int value, int* command, int* operand);
void sc_memoryOutput(void);
void sc_memoryAddressOutput(int x, int y);
int sc_memoryFree(void);
