#include <sc/helper.h>
#include <sc/ram-operations.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 100

int getCommand(char* s) {
    int c = -1;

    if (!strcmp(s, "READ"))
        c = 0x10;
    else if (!strcmp(s, "WRITE"))
        c = 0x11;
    else if (!strcmp(s, "LOAD"))
        c = 0x20;
    else if (!strcmp(s, "STORE"))
        c = 0x21;
    else if (!strcmp(s, "JUMP"))
        c = 0x40;
    else if (!strcmp(s, "JNEG"))
        c = 0x41;
    else if (!strcmp(s, "JZ"))
        c = 0x42;
    else if (!strcmp(s, "HALT"))
        c = 0x43;
    else if (!strcmp(s, "ADD"))
        c = 0x30;
    else if (!strcmp(s, "SUB"))
        c = 0x31;
    else if (!strcmp(s, "DIVIDE"))
        c = 0x32;
    else if (!strcmp(s, "MUL"))
        c = 0x33;
    else if (!strcmp(s, "RCL"))
        c = 0x62;
    else if (!strcmp(s, "="))
        c = 0x00;

    return c;
}

void del_newline_in_str(char* s) {
    if (s[strlen(s) - 1] == '\n') s[strlen(s) - 1] = 0;
}

int sat(const char* filepath, const char* result) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "sat:\e[31m fatal error:\e[39m %s: There is no such file or directory\n", filepath);
        return ERROR_CODE;
    }
    int memory[MEMORY_SIZE];
    memset(memory, 0, MEMORY_SIZE);
    int count_strs = 1;
    while (!feof(file)) {
        char buf[BUFSIZ] = {0};
        fgets(buf, BUFSIZ, file);
        if (strlen(buf) < 1) break;

        del_newline_in_str(buf);

        char* tok = strtok(buf, " ");  //* get index memory
        int i = atoi(tok);
        if ((!i && strcmp(tok, "00")) || i >= MEMORY_SIZE) {
            fprintf(stderr, "sat:%d\e[31m fatal error:\e[39m %s: Out of range\n", count_strs, tok);
            return ERROR_CODE;
        }

        tok = strtok(NULL, " ");  //* get command
        int command = getCommand(tok);
        if (command == -1) {
            fprintf(stderr, "sat:%d\e[31m fatal error:\e[39m %s: Unknown command\n", count_strs, tok);
            return ERROR_CODE;
        }

        tok = strtok(NULL, " ");  //* get operand
        char* tok_tmp = *tok == '+' || *tok == '-' ? tok + 1 : tok;
        int operand = atoi(tok_tmp);
        if (!operand && (strcmp(tok_tmp, "00") && strcmp(tok_tmp, "0000"))) {
            fprintf(stderr, "sat:%d\e[31m fatal error:\e[39m %s: Invalid operand\n", count_strs, tok);
            return ERROR_CODE;
        }

        if (*tok == '-') operand = -operand;

        tok = strtok(NULL, " ");  //* get comment
        if (tok && *tok != ';') {
            fprintf(stderr, "sat:%d\e[31m syntax error:\e[39m %s: The ';' character was expected\n",
                    count_strs, tok);
            return ERROR_CODE;
        }

        if (!command) {
            if (operand < 0)
                memory[i] = ((-operand) & 0x3fff) | 0x4000;
            else
                memory[i] = operand & 0x3fff;
        } else {
            if (sc_commandEncode(command, operand, &memory[i])) {
                fprintf(stderr, "sat:%d\e[31m fatal error:\e[39m sc_commandEncode(): -1\n", count_strs);
                return ERROR_CODE;
            }
        }
        ++count_strs;
    }
    fclose(file);

    FILE* data = fopen(result, "wb");
    if (!data) {
        fprintf(stderr, "sat:\e[31m fatal error:\e[39m %s: Failed to save result\n", filepath);
        return ERROR_CODE;
    }
    fwrite(memory, sizeof(memory[0]), MEMORY_SIZE, data);
    fclose(data);

    return SUCCES_CODE;
}

int main(int argc, char** argv) {
    if (argc < 3) return ERROR_CODE;
    return sat(argv[1], argv[2]);
}
