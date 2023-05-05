#include <ctype.h>
#include <sc/helper.h>
#include <sc/ram-operations.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma region abstract_syntax_tree

typedef enum { OPERATOR, OPERAND, STATEMENT } node_type_t;

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
int is_count(char* str) {
    if (str && (*str == '+' || *str == '-')) ++str;
    while (str && *str) {
        if (!isdigit(*str)) return 0;
        ++str;
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

int var_getvalue(variables* vars, char* name) {
    for (int i = 99; i >= 0 && vars[i].name; --i) {
        if (!strcmp(vars[i].name, name)) {
            return vars[i].value;
        }
    }
    return -0xffff;
}

int var_setvalue(variables* vars, char* name, int val) {
    for (int i = 99; i >= 0 && vars[i].name; --i) {
        if (!strcmp(vars[i].name, name)) {
            vars[i].value = val;
            return 0;
        }
    }
    return -1;
}

int add_count_in_stack(variables* vars, int* vars_size, char* name, int value) {
    if (var_getaddress(vars, name) == -1) {
        variables_push_back(vars, *vars_size, name, value);
        return 0;
    }
    return -1;
}

#pragma endregion  // abstract_syntax_tree

#pragma region static_stack

#define STACK_SIZE 100

typedef struct static_stack {
    char* data;
    int top;
} static_stack;

typedef struct simple_basic_string {
    static_stack* root;
    variables* var;
    char* operation;
    int n;
} sbstring;

static_stack* create_stack() {
    static_stack* s = (static_stack*)malloc(sizeof(*s));
    if (s) {
        s->top = -1;
        s->data = (char*)calloc(STACK_SIZE, 1);
    }
    return s;
}

void static_stack_push(static_stack* s, char val) {
    if (s->top >= STACK_SIZE - 1) {
        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Stack overflow\n");
        exit(EXIT_FAILURE);
    }
    s->data[++(s->top)] = val;
}

char static_stack_pop(static_stack* s) {
    if (s->top < 0) {
        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Stack underflow\n");
        exit(EXIT_FAILURE);
    }
    return s->data[s->top--];
}

typedef struct dynamic_stack {
    char* data;
    struct dynamic_stack* next;
} stack;

void push(stack** top, char* x) {
    stack* temp = (stack*)malloc(sizeof(*temp));
    temp->data = strdup(x);
    temp->next = *top;
    *top = temp;
}

char* pop(stack** top) {
    if (*top == NULL) {
        printf("Stack is empty.\n");
        return NULL;
    }
    stack* temp = *top;
    char* ret = (*top)->data;
    *top = (*top)->next;
    free(temp);
    return ret;
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

char* to_postfix(char* expr, static_stack* s) {
    char* postfix = (char*)malloc(strlen(expr) + 1);
    char* p = postfix;
    for (char* q = expr; *q; q++) {
        if (isspace(*q)) {
            continue;
        } else if (isdigit(*q) || (*q == '-' && isdigit(*(q + 1)))) {
            *p++ = *q;
            q++;
            while (isdigit(*q) || *q == '.') {
                *p++ = *q;
                q++;
            }
            *p++ = ' ';
            q--;
        } else if (is_operator(*q)) {
            while (s->top >= 0 && is_operator(s->data[s->top]) &&
                   precedence(s->data[s->top]) >= precedence(*q)) {
                *p++ = static_stack_pop(s);
                *p++ = ' ';
            }
            static_stack_push(s, *q);
        } else {
            *p++ = *q;
            while (*(q + 1) && !isspace(*(q + 1)) && !is_operator(*(q + 1))) {
                *p++ = *++q;
            }
            *p++ = ' ';
        }
    }
    while (s->top >= 0) {
        *p++ = static_stack_pop(s);
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
        if (!is_all_uppercase(tok)) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: Invalid variable name: %s\n", s, tok);
            return ERROR_CODE;
        }
        if (var_getaddress(vars, tok) == -1) variables_push_back(vars, *vars_size, tok, 0);
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
        if (!tok || (strcmp(tok, ">") && strcmp(tok, "<") && strcmp(tok, "="))) {
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
        if (!is_all_uppercase(tok) && !is_count(tok)) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: Invalid variable name\n", s);
            return ERROR_CODE;
        }
        if (var_getaddress(vars, tok) == -1) variables_push_back(vars, *vars_size, tok, 0);
        if ((tok = strtok(NULL, " "))) {
            if (!strcmp(tok, "=")) {
                while ((tok = strtok(NULL, " "))) {
                    del_newline_in_str(tok);
                    if (!strcmp(tok, "+") || !strcmp(tok, "-")) {
                    } else if (!strcmp(tok, "*") || !strcmp(tok, "/")) {
                    } else if (is_all_uppercase(tok) || is_count(tok)) {
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

#pragma endregion  // static_stack

typedef struct simple_assembler_instruction {
    char instruction[8];
    int n;
    int operand;
    int n_sb;
} sbinstruction;

int get_sa_address(sbinstruction* si, int instructionSize, int n_sb) {
    for (int i = 0; i < instructionSize; ++i) {
        if (si[i].n_sb == n_sb) return si[i].n;
    }
    return -1;
}

int to_sa(char* str, variables* vars, int* vars_size, int ni, sbinstruction* si, int* instructionSize) {
    static int instructionCounter = 0;

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
        si[*instructionSize].n_sb = ni;
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
        si[*instructionSize].n_sb = ni;
        *instructionSize += 1;
    } else if (!strcmp(tok, "GOTO")) {
        strcpy(si[*instructionSize].instruction, "JUMP");
        tok = strtok(NULL, " ");
        int addr = atoi(tok);
        int sa_addr;
        if ((!addr && *tok != '0') || (sa_addr = get_sa_address(si, *instructionSize, addr)) == -1) {
            fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Invalid address: %s\n", tok);
            return ERROR_CODE;
        }
        si[*instructionSize].n = instructionCounter++;
        si[*instructionSize].operand = sa_addr;
        si[*instructionSize].n_sb = ni;
        *instructionSize += 1;
    } else if (!strcmp(tok, "IF")) {
        tok = strtok(NULL, " ");  //* get operand
        int addr;
        if (is_count(tok)) {
            fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Inlavid lvalue: %s\n", tok);
            return ERROR_CODE;
        }
        if ((addr = var_getaddress(vars, tok)) == -1) {
            fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
            return ERROR_CODE;
        }
        tok = strtok(NULL, " ");  //* get operation
        strcpy(si[*instructionSize].instruction, "LOAD");
        si[*instructionSize].n = instructionCounter++;
        si[*instructionSize].operand = addr;
        si[*instructionSize].n_sb = ni;
        *instructionSize += 1;
        if (!strcmp(tok, ">")) {
            tok = strtok(NULL, " ");  //* get operand
            if (var_getaddress(vars, tok) == -1 && is_count(tok) && strcmp(tok, "0"))
                add_count_in_stack(vars, vars_size, tok, atoi(tok));
            if (!strcmp(tok, "0")) {
                strcpy(bf, str);
                tok = strstr(bf, "0") + 1;
                int cur_i = *instructionSize;
                strcpy(si[*instructionSize].instruction, "JNEG");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].n_sb = ni;
                *instructionSize += 1;
                to_sa(tok, vars, vars_size, ni, si, instructionSize);
                si[cur_i].operand = instructionCounter;
            } else {
                int addr_cmp;
                if ((addr_cmp = var_getaddress(vars, tok)) == -1) {
                    fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
                    return ERROR_CODE;
                }
                strcpy(si[*instructionSize].instruction, "SUB");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].n_sb = ni;
                si[*instructionSize].operand = addr_cmp;
                *instructionSize += 1;
                strcpy(bf, str);
                tok = strstr(bf, tok) + 1;
                int cur_i = *instructionSize;
                strcpy(si[*instructionSize].instruction, "JNEG");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].n_sb = ni;
                *instructionSize += 1;
                to_sa(tok, vars, vars_size, ni, si, instructionSize);
                si[cur_i].operand = instructionCounter;
            }
        } else if (!strcmp(tok, "<")) {
            tok = strtok(NULL, " ");  //* get operand
            if (var_getaddress(vars, tok) == -1 && is_count(tok) && strcmp(tok, "0"))
                add_count_in_stack(vars, vars_size, tok, atoi(tok));
            if (!strcmp(tok, "0")) {
                strcpy(bf, str);
                tok = strstr(bf, "0") + 1;
                strcpy(si[*instructionSize].instruction, "JNEG");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].operand = instructionCounter + 1;
                si[*instructionSize].n_sb = ni;
                *instructionSize += 1;
                strcpy(si[*instructionSize].instruction, "JUMP");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].n_sb = ni;
                int cur_i = *instructionSize;
                *instructionSize += 1;
                to_sa(tok, vars, vars_size, ni, si, instructionSize);
                si[cur_i].operand = instructionCounter;
            } else {
                int addr_cmp;
                if ((addr_cmp = var_getaddress(vars, tok)) == -1) {
                    fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
                    return ERROR_CODE;
                }
                strcpy(si[*instructionSize].instruction, "SUB");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].n_sb = ni;
                si[*instructionSize].operand = addr_cmp;
                *instructionSize += 1;
                strcpy(bf, str);
                tok = strstr(bf, tok) + 1;
                strcpy(si[*instructionSize].instruction, "JNEG");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].operand = instructionCounter + 1;
                si[*instructionSize].n_sb = ni;
                *instructionSize += 1;
                strcpy(si[*instructionSize].instruction, "JUMP");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].n_sb = ni;
                int cur_i = *instructionSize;
                *instructionSize += 1;
                to_sa(tok, vars, vars_size, ni, si, instructionSize);
                si[cur_i].operand = instructionCounter;
            }
        } else if (!strcmp(tok, "=")) {
            tok = strtok(NULL, " ");  //* get operand
            if (var_getaddress(vars, tok) == -1 && is_count(tok) && strcmp(tok, "0"))
                add_count_in_stack(vars, vars_size, tok, atoi(tok));
            if (!strcmp(tok, "0")) {
                strcpy(bf, str);
                tok = strstr(bf, "0") + 1;
                strcpy(si[*instructionSize].instruction, "JZ");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].operand = instructionCounter + 1;
                si[*instructionSize].n_sb = ni;
                *instructionSize += 1;
                strcpy(si[*instructionSize].instruction, "JUMP");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].n_sb = ni;
                int cur_i = *instructionSize;
                *instructionSize += 1;
                to_sa(tok, vars, vars_size, ni, si, instructionSize);
                si[cur_i].operand = instructionCounter;
            } else {
                int addr_cmp;
                if ((addr_cmp = var_getaddress(vars, tok)) == -1) {
                    fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
                    return ERROR_CODE;
                }
                strcpy(si[*instructionSize].instruction, "SUB");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].n_sb = ni;
                si[*instructionSize].operand = addr_cmp;
                *instructionSize += 1;
                strcpy(bf, str);
                tok = strstr(bf, tok) + 1;
                strcpy(si[*instructionSize].instruction, "JZ");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].operand = instructionCounter + 1;
                si[*instructionSize].n_sb = ni;
                *instructionSize += 1;
                strcpy(si[*instructionSize].instruction, "JUMP");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].n_sb = ni;
                int cur_i = *instructionSize;
                *instructionSize += 1;
                to_sa(tok, vars, vars_size, ni, si, instructionSize);
                si[cur_i].operand = instructionCounter;
            }
        } else {
            fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Invalid operation: %s\n", tok);
            return ERROR_CODE;
        }
    } else if (!strcmp(tok, "LET")) {
        tok = strtok(NULL, " ");
        char* name = tok;
        if ((tok = strtok(NULL, " ")) && !strcmp(tok, "=")) {
            int addr;
            if ((addr = var_getaddress(vars, name)) == -1) {
                fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
                return ERROR_CODE;
            }
            tok = strtok(NULL, " ");
            int addr_start;
            if (is_count(tok)) add_count_in_stack(vars, vars_size, tok, atoi(tok));
            if ((addr_start = var_getaddress(vars, tok)) == -1) {
                fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
                return ERROR_CODE;
            }

            stack* stck = NULL;
            push(&stck, tok);
            bool load = false;
            while ((tok = strtok(NULL, " "))) {
                if (!load && (!strcmp(tok, "+") || !strcmp(tok, "-") || !strcmp(tok, "*") ||
                              !strcmp(tok, "/"))) {  //? load in accumulator
                    load = true;
                    char* b = pop(&stck);
                    char* a = pop(&stck);
                    addr_start = var_getaddress(vars, a);
                    strcpy(si[*instructionSize].instruction, "LOAD");
                    si[*instructionSize].n = instructionCounter++;
                    si[*instructionSize].operand = addr_start;
                    si[*instructionSize].n_sb = ni;
                    *instructionSize += 1;
                    push(&stck, a);
                    push(&stck, b);
                    free(a);
                    free(b);
                }
                if (!strcmp(tok, "+")) {
                    char* a = pop(&stck);
                    char* b = pop(&stck);
                    addr_start = var_getaddress(vars, a);
                    strcpy(si[*instructionSize].instruction, "ADD");
                    si[*instructionSize].n = instructionCounter++;
                    si[*instructionSize].operand = addr_start;
                    si[*instructionSize].n_sb = ni;
                    *instructionSize += 1;
                    push(&stck, name);
                    free(a);
                    free(b);
                } else if (!strcmp(tok, "-")) {
                    char* a = pop(&stck);
                    char* b = pop(&stck);
                    addr_start = var_getaddress(vars, a);
                    strcpy(si[*instructionSize].instruction, "SUB");
                    si[*instructionSize].n = instructionCounter++;
                    si[*instructionSize].operand = addr_start;
                    si[*instructionSize].n_sb = ni;
                    *instructionSize += 1;
                    push(&stck, name);
                    free(a);
                    free(b);
                } else if (!strcmp(tok, "*")) {
                    char* a = pop(&stck);
                    char* b = pop(&stck);
                    addr_start = var_getaddress(vars, a);
                    strcpy(si[*instructionSize].instruction, "MUL");
                    si[*instructionSize].n = instructionCounter++;
                    si[*instructionSize].operand = addr_start;
                    si[*instructionSize].n_sb = ni;
                    *instructionSize += 1;
                    push(&stck, name);
                    free(a);
                    free(b);
                } else if (!strcmp(tok, "/")) {
                    char* a = pop(&stck);
                    char* b = pop(&stck);
                    addr_start = var_getaddress(vars, a);
                    strcpy(si[*instructionSize].instruction, "DIVIDE");
                    si[*instructionSize].n = instructionCounter++;
                    si[*instructionSize].operand = addr_start;
                    si[*instructionSize].n_sb = ni;
                    *instructionSize += 1;
                    push(&stck, name);
                    free(a);
                    free(b);
                } else {
                    if ((addr_start = var_getaddress(vars, tok)) == -1 && !is_count(tok)) {
                        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m: Undeclared variable: %s\n", tok);
                        return ERROR_CODE;
                    } else {
                        if (is_count(tok)) add_count_in_stack(vars, vars_size, tok, atoi(tok));
                        push(&stck, tok);
                    }
                }
            }
            if (!load) {
                strcpy(si[*instructionSize].instruction, "LOAD");
                si[*instructionSize].n = instructionCounter++;
                si[*instructionSize].operand = addr_start;
                si[*instructionSize].n_sb = ni;
                *instructionSize += 1;
                load = true;
            }
            strcpy(si[*instructionSize].instruction, "STORE");
            si[*instructionSize].n = instructionCounter++;
            si[*instructionSize].operand = addr;
            si[*instructionSize].n_sb = ni;
            *instructionSize += 1;
        }
    } else if (!strcmp(tok, "END")) {
        strcpy(si[*instructionSize].instruction, "HALT");
        si[*instructionSize].n = instructionCounter++;
        si[*instructionSize].operand = 0;
        si[*instructionSize].n_sb = ni;
        *instructionSize += 1;
    } else {
        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m %s: Unknown instruction\n", tok);
        return ERROR_CODE;
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
        strs[i].root->data = to_postfix(tok, strs[i].root);
        tok = strtok(tok, " ");  //* get instruction
        if (build_stack(tok, &strs[i], var, &var_size)) {
            fprintf(stderr, "error line: %d\nexit code: 1\n", i);
            exit(1);
        }
    }

    sbinstruction si[100];
    int instructionSize = 0;
    for (int i = 0; strs[i].root; ++i) {
        if (to_sa(strs[i].root->data, var, &var_size, strs[i].n, si, &instructionSize) == ERROR_CODE) {
            fprintf(stderr, "sbt: translate error: %d\n", i + 1);
        }
    }

    typedef struct {
        int n;
        int old;
    } pair_int;

    pair_int new_idxs[99 - var_size];
    for (int i = 0, j = 99, last_i = instructionSize; j > var_size; ++i, --j) {
        new_idxs[i].n = last_i++;
        new_idxs[i].old = var[j].address;
        var[j].address = new_idxs[i].n;
    }
    for (int i = 0; i < 99 - var_size; ++i) {
        for (int j = 0; j < 100; ++j) {
            if (new_idxs[i].old == si[j].operand) si[j].operand = new_idxs[i].n;
        }
    }

    FILE* file_sa = fopen(result, "w");
    for (int i = 0; i < 100; ++i) {
        if (si[i].instruction[0]) {
            fprintf(file_sa, "%02d %s %02d\n", si[i].n, si[i].instruction, si[i].operand);
        }
    }
    for (int i = 99; i >= 0 && var[i].name; --i) {
        char sign = var[i].value < 0 ? '-' : '+';
        fprintf(file_sa, "%02d %s    %c%04X\n", var[i].address, "=", sign, abs(var[i].value));
    }
    fclose(file_sa);

    for (int i = 0; strs[i].root; ++i) {
        printf("%s\n", strs[i].root->data);
    }

    for (int i = 0; strs[i].root; ++i) {
        free(strs[i].root->data);
        free(strs[i].root);
    }

    return SUCCES_CODE;
}

int main(int argc, char** argv) {
    if (argc < 3) return ERROR_CODE;
    return sbt(argv[1], argv[2]);
}
