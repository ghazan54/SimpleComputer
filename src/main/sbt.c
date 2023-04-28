#include <ctype.h>
#include <sc/helper.h>
#include <sc/ram-operations.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma region abstract_syntax_tree

typedef struct abstract_syntax_tree ast_t;

struct abstract_syntax_tree {
    ast_t* left;
    ast_t* right;
    char* data;
    int priority;
    int node_type;
};

typedef enum { OPERATOR, OPERAND, STATEMENT } node_type_t;

typedef enum {
    priority_io = 0,
    priority_if = 2,
    priority_goto = 1,
    priority_let = 3,
    priority_cnd_statement = 4,
    priority_operator_low = 5,
    priority_operator_high = 6,
    priority_operand = 10
} priorities;

typedef struct simple_basic_variables {
    char* name;
    int value;
    int address;
} variables;

#define variables_push_back(_var, _size, _name, _value) \
    {                                                   \
        _var[_size].address = _size;                    \
        _var[_size].name = strdup(_name);               \
        _var[_size].value = _value;                     \
        _size -= 1;                                     \
    }

int is_all_uppercase(char* str) {
    int i = 0;
    while (str[i]) {
        if (str[i] < 'A' || str[i] > 'Z' || !isupper(str[i])) {
            return 0;
        }
        ++i;
    }
    return 1;
}

void del_newline_in_str(char* s) {
    if (s[strlen(s) - 1] == '\n') s[strlen(s) - 1] = 0;
}

int var_getaddress(variables* vars, char* name) {
    for (int i = 99; i >= 0 && vars[i].name; --i) {
        if (!strcmp(vars[i].name, name)) {
            return vars[i].address;
        }
    }
    return -1;
}

#pragma endregion  // abstract_syntax_tree

#pragma region stack

#define STACK_SIZE 100

typedef struct {
    char* stack;
    int top;
} stack;

typedef struct simple_basic_string {
    stack* root;
    variables* var;
    char* operation;
    int n;
} sbstring;

stack* create_stack() {
    stack* s = (stack*)malloc(sizeof(*s));
    if (s) {
        s->top = -1;
        s->stack = (char*)calloc(STACK_SIZE, 1);
    }
    return s;
}

void push(stack* s, char val) {
    if (s->top >= STACK_SIZE - 1) {
        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Stack overflow\n");
        exit(EXIT_FAILURE);
    }
    s->stack[++(s->top)] = val;
}

char pop(stack* s) {
    if (s->top < 0) {
        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Stack underflow\n");
        exit(EXIT_FAILURE);
    }
    return s->stack[s->top--];
}

int is_operator(char c) { return c == '+' || c == '-' || c == '*' || c == '/'; }

int precedence(char op) {
    switch (op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        default:
            return 0;
    }
}

char* to_postfix(char* expr, stack* s) {
    char* postfix = (char*)malloc(strlen(expr) + 1);
    char* p = postfix;
    for (char* q = expr; *q; q++) {
        if (isspace(*q)) {
            continue;
        } else if (isdigit(*q)) {
            *p++ = *q;
            while (isdigit(*++q) || *q == '.') {
                *p++ = *q;
            }
            *p++ = ' ';
            --q;
        } else if (is_operator(*q)) {
            while (s->top >= 0 && is_operator(s->stack[s->top]) &&
                   precedence(s->stack[s->top]) >= precedence(*q)) {
                *p++ = pop(s);
                *p++ = ' ';
            }
            push(s, *q);
        } else {
            *p++ = *q;
            while (*(q + 1) && !isspace(*(q + 1)) && !is_operator(*(q + 1))) {
                *p++ = *++q;
            }
            *p++ = ' ';
        }
    }
    while (s->top >= 0) {
        *p++ = pop(s);
        *p++ = ' ';
    }
    *p = '\0';
    return postfix;
}

int build_stack(char* s, sbstring* str, variables* vars, int* vars_size) {
    if (!strcmp(s, "REM")) {
    } else if (!strcmp(s, "INPUT")) {
        char* tok = strtok(NULL, " ");
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The operand was expected\n", s);
            return ERROR_CODE;
        }
        del_newline_in_str(tok);
        variables_push_back(vars, *vars_size, tok, 0);
        tok = strtok(NULL, " ");
        if (tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The end of the line was expected\n", s);
            return ERROR_CODE;
        }
    } else if (!strcmp(s, "PRINT")) {
        char* tok = strtok(NULL, " ");
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The operand was expected\n", s);
            return ERROR_CODE;
        }
        del_newline_in_str(tok);
        tok = strtok(NULL, " ");
        if (tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The end of the line was expected\n", s);
            return ERROR_CODE;
        }
    } else if (!strcmp(s, "GOTO")) {
        char* tok = strtok(NULL, " ");
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The operand was expected\n", s);
            return ERROR_CODE;
        }
        del_newline_in_str(tok);
        tok = strtok(NULL, " ");
        if (tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The end of the line was expected\n", s);
            return ERROR_CODE;
        }
    } else if (!strcmp(s, "IF")) {
        char* tok = strtok(NULL, " ");  //* get operand
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The operand was expected\n", s);
            return ERROR_CODE;
        }
        tok = strtok(NULL, " ");  //* get operator
        if (!tok || (strcmp(tok, ">") && strcmp(tok, "<"))) {
            fprintf(stderr,
                    "sbt:\e[31m syntax error:\e[39m %s: Conditional operator ('>' or '<') was expected\n",
                    tok);
            return ERROR_CODE;
        }
        tok = strtok(NULL, " ");
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The operand was expected\n", s);
            return ERROR_CODE;
        }
        tok = strtok(NULL, " ");
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: Expected expression in the 'if' construct\n",
                    s);
            return ERROR_CODE;
        }
        if (build_stack(tok, str, vars, vars_size) == ERROR_CODE) return ERROR_CODE;
    } else if (!strcmp(s, "LET")) {
        char* tok = strtok(NULL, " ");
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The operand was expected\n", s);
            return ERROR_CODE;
        }
        del_newline_in_str(tok);
        variables_push_back(vars, *vars_size, tok, 0);
        if ((tok = strtok(NULL, " "))) {
            if (!strcmp(tok, "=")) {
                while ((tok = strtok(NULL, " "))) {
                    del_newline_in_str(tok);
                    if (!strcmp(tok, "+") || !strcmp(tok, "-")) {
                    } else if (!strcmp(tok, "*") || !strcmp(tok, "/")) {
                    } else if (is_all_uppercase(tok)) {
                    } else {
                        fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: Invalid name '%s'\n", s, tok);
                        return ERROR_CODE;
                    }
                }
            } else {
                fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: Expected '=', found '%s'\n", s, tok);
                return ERROR_CODE;
            }
        }
    } else if (!strcmp(s, "END")) {
        char* tok = strtok(NULL, " ");
        tok = strtok(NULL, " ");
        if (tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The end of the line was expected\n", s);
            return ERROR_CODE;
        }
    } else {
        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m %s: Unknown instruction\n", s);
        return ERROR_CODE;
    }
    return SUCCES_CODE;
}

#pragma endregion  // stack

typedef struct simple_assembler_instruction {
    char instruction[8];
    int n;
    int operand;
} sbinstruction;

int to_sa(char* str, variables* vars, int* vars_size, sbinstruction* si, int* instructionSize) {
    static int instructionCounter = 1;

    char bf[BUFSIZ];
    strcpy(bf, str);

    char* tok = strtok(bf, " ");
    if (!strcmp(tok, "REM")) {
        fprintf(stderr, "sbt:\e[38;5;206m warning\e[39m: Comments ignored\n");
    } else if (!strcmp(tok, "INPUT")) {
        strcpy(si[*instructionSize].instruction, "READ");
        tok = strtok(NULL, " ");
        int addr;
        if ((addr = var_getaddress(vars, tok)) == -1) {
            fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
            return ERROR_CODE;
        }
        si[*instructionSize].n = instructionCounter++;
        si[*instructionSize].operand = addr;
        *instructionSize += 1;
    } else if (!strcmp(tok, "PRINT")) {
        strcpy(si[*instructionSize].instruction, "WRITE");
        tok = strtok(NULL, " ");
        int addr;
        if ((addr = var_getaddress(vars, tok)) == -1) {
            fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
            return ERROR_CODE;
        }
        si[*instructionSize].n = instructionCounter++;
        si[*instructionSize].operand = addr;
        *instructionSize += 1;
    } else if (!strcmp(tok, "GOTO")) {
        strcpy(si[*instructionSize].instruction, "JUMP");
        tok = strtok(NULL, " ");
        int addr = atoi(tok);
        if (!addr && *tok != '0') {
            fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Invalid address: %s\n", tok);
            return ERROR_CODE;
        }
        si[*instructionSize].n = instructionCounter++;
        si[*instructionSize].operand = addr;
        *instructionSize += 1;
    } else if (!strcmp(tok, "IF")) {
    } else if (!strcmp(tok, "LET")) {
        strcpy(bf, str);
        tok = strtok(bf, " ");
        tok = strtok(NULL, "\n");
        if (to_sa(tok, vars, vars_size, si, instructionSize) == ERROR_CODE) return ERROR_CODE;
    } else if (!strcmp(tok, "END")) {
        strcpy(si[*instructionSize].instruction, "HALT");
        si[*instructionSize].n = instructionCounter++;
        si[*instructionSize].operand = 0;
        *instructionSize += 1;
    } else {
        char* name = tok;
        if ((tok = strtok(NULL, " ")) && !strcmp(tok, "=")) {
            int addr;
            if ((addr = var_getaddress(vars, name)) == -1) {
                fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
                return ERROR_CODE;
            }
            tok = strtok(NULL, " ");
            int addr_start;
            if ((addr_start = var_getaddress(vars, tok)) == -1) {
                fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
                return ERROR_CODE;
            }
            strcpy(si[*instructionSize].instruction, "LOAD");
            si[*instructionSize].n = instructionCounter++;
            si[*instructionSize].operand = addr_start;
            *instructionSize += 1;
            char* lname = tok;
            while ((tok = strtok(NULL, " "))) {
                if (!strcmp(tok, "+")) {
                    if ((addr_start = var_getaddress(vars, lname)) == -1) {
                        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
                        return ERROR_CODE;
                    }
                    strcpy(si[*instructionSize].instruction, "ADD");
                    si[*instructionSize].n = instructionCounter++;
                    si[*instructionSize].operand = addr_start;
                    *instructionSize += 1;
                } else if (!strcmp(tok, "-")) {
                    if ((addr_start = var_getaddress(vars, lname)) == -1) {
                        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
                        return ERROR_CODE;
                    }
                    strcpy(si[*instructionSize].instruction, "SUB");
                    si[*instructionSize].n = instructionCounter++;
                    si[*instructionSize].operand = addr_start;
                    *instructionSize += 1;
                } else if (!strcmp(tok, "*")) {
                    if ((addr_start = var_getaddress(vars, lname)) == -1) {
                        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
                        return ERROR_CODE;
                    }
                    strcpy(si[*instructionSize].instruction, "MUL");
                    si[*instructionSize].n = instructionCounter++;
                    si[*instructionSize].operand = addr_start;
                    *instructionSize += 1;
                } else if (!strcmp(tok, "/")) {
                    if ((addr_start = var_getaddress(vars, lname)) == -1) {
                        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
                        return ERROR_CODE;
                    }
                    strcpy(si[*instructionSize].instruction, "DIVIDE");
                    si[*instructionSize].n = instructionCounter++;
                    si[*instructionSize].operand = addr_start;
                    *instructionSize += 1;
                } else {
                    lname = tok;
                }
            }
            strcpy(si[*instructionSize].instruction, "STORE");
            si[*instructionSize].n = instructionCounter++;
            si[*instructionSize].operand = addr;
            *instructionSize += 1;
        } else {
            fprintf(stderr, "sbt:\e[31m fatal error:\e[39m %s: Unknown instruction\n", tok);
            return ERROR_CODE;
        }
    }
    return SUCCES_CODE;
}

int sbt(const char* filepath, const char* result) {
    FILE* file = fopen(filepath, "r");

    if (!file) {
        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m %s: There is no such file or directory\n", filepath);
        return ERROR_CODE;
    }

    sbstring strs[256];
    variables var[100];
    int var_size = 99;

    //? init variables array
    for (int i = 0; i < (int)(sizeof(var) / sizeof(var[0])); ++i)
        var[i].name = NULL, var[i].value = 0xffff, var[i].address = -1;

    for (int i = 0; !feof(file); ++i) {
        char bf[BUFSIZ] = {0};
        fgets(bf, BUFSIZ, file);
        strs[i].var = var;
        strs[i].root = create_stack();
        char* tok = strtok(bf, " ");  //* get instruction number
        strs[i].n = atoi(tok);
        if (strs[i].n == 0 && (strcmp(tok, "00") || strcmp(tok, "000"))) {
            fprintf(stderr, "sbt:\e[31m fatal error:\e[39m %s: Invalid instruction number\n", tok);
            return ERROR_CODE;
        }

        tok = strtok(NULL, "\n");  //* get full line
        strs[i].root->stack = to_postfix(tok, strs[i].root);
        tok = strtok(tok, " ");  //* get instruction
        if (build_stack(tok, &strs[i], var, &var_size)) {
            fprintf(stderr, "error line: %d\nexit code: 1\n", i);
            exit(1);
        }
    }

    sbinstruction si[100];
    int instructionSize = 0;
    for (int i = 0; strs[i].root; ++i) {
        to_sa(strs[i].root->stack, var, &var_size, si, &instructionSize);
    }

    FILE* file_sa = fopen(result, "w");
    for (int i = 0; i < 100; ++i) {
        if (si[i].instruction[0])
            fprintf(file_sa, "%02d %s %02d\n", si[i].n, si[i].instruction, si[i].operand);
    }
    fclose(file_sa);

    // for (int i = 0; strs[i].root; ++i) {
    //     printf("%s\n", strs[i].root->stack);
    // }

    for (int i = 0; strs[i].root; ++i) {
        free(strs[i].root->stack);
        free(strs[i].root);
    }

    return SUCCES_CODE;
}

int main(int argc, char** argv) {
    if (argc < 3) return ERROR_CODE;
    return sbt(argv[1], argv[2]);
}
