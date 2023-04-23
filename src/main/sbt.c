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
    priority_if = 1,
    priority_goto = 2,
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

typedef struct simple_basic_string {
    ast_t* root;
    variables* var;
    int n;
} sbstring;

ast_t* ast_create(int priority, char* data, int node_type) {
    ast_t* root = (ast_t*)malloc(sizeof(*root));
    if (root) {
        root->data = strdup(data);
        root->left = root->right = NULL;
        root->priority = priority;
        root->node_type = node_type;
    }
    return root;
}

ast_t* ast_insert(ast_t* root, int priority, char* data, int node_type) {
    if (!root) {
        root = ast_create(priority, data, node_type);
    } else if (root->priority < priority) {
        root->right = ast_insert(root->right, priority, data, node_type);
    } else {
        ast_t* node = ast_create(priority, data, node_type);
        node->left = root;
        return node;
    }
    return root;
}

void ast_free(ast_t* root) {
    if (root) {
        if (root->left) ast_free(root->left);
        if (root->right) ast_free(root->right);
        free(root->data);
        free(root);
    }
}

void __ast_print(char* c, int p, int s) {
    int t = s;
    for (int i = 0; i < p; i++) {
        printf(t & 1 ? "|  " : "   ");
        t /= 2;
    }
    printf("%s", c);
}

void _ast_print(ast_t* tree, int p, int s) {
    if (!tree) return;

    printf("%s", tree->data);
    printf("\n");

    if (tree->left) {
        __ast_print("|\n", p, s);
        __ast_print("L: ", p, s);
        _ast_print(tree->left, p + 1, s + ((tree->right == NULL ? 0 : 1) << p));
    }
    if (tree->right) {
        __ast_print("|\n", p, s);
        __ast_print("R: ", p, s);
        _ast_print(tree->right, p + 1, s);
    }
}

void ast_print(ast_t* tree) { _ast_print(tree, 0, 0); }

#pragma endregion  // abstract_syntax_tree

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

int build_ast(char* s, sbstring* str, variables* vars, int* vars_size) {
    if (!strcmp(s, "REM")) {
        str->root = ast_insert(str->root, -1, s, STATEMENT);
    } else if (!strcmp(s, "INPUT")) {
        str->root = ast_insert(str->root, priority_io, s, STATEMENT);
        char* tok = strtok(NULL, " ");
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The operand was expected\n", s);
            return ERROR_CODE;
        }
        del_newline_in_str(tok);
        str->root = ast_insert(str->root, priority_operand, tok, OPERAND);
        variables_push_back(vars, *vars_size, tok, 0);
        tok = strtok(NULL, " ");
        if (tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The end of the line was expected\n", s);
            return ERROR_CODE;
        }
    } else if (!strcmp(s, "PRINT")) {
        str->root = ast_insert(str->root, priority_io, s, STATEMENT);
        char* tok = strtok(NULL, " ");
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The operand was expected\n", s);
            return ERROR_CODE;
        }
        str->root = ast_insert(str->root, priority_operand, tok, OPERAND);
        tok = strtok(NULL, " ");
        if (tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The end of the line was expected\n", s);
            return ERROR_CODE;
        }
    } else if (!strcmp(s, "GOTO")) {
        str->root = ast_insert(str->root, priority_goto, s, STATEMENT);
        char* tok = strtok(NULL, " ");
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The operand was expected\n", s);
            return ERROR_CODE;
        }
        str->root = ast_insert(str->root, priority_operand, tok, OPERAND);
        tok = strtok(NULL, " ");
        if (tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The end of the line was expected\n", s);
            return ERROR_CODE;
        }
    } else if (!strcmp(s, "IF")) {
        str->root = ast_insert(str->root, priority_if, s, STATEMENT);
        char* tok = strtok(NULL, " ");  //* get operand
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The operand was expected\n", s);
            return ERROR_CODE;
        }
        str->root = ast_insert(str->root, priority_operand, tok, OPERAND);
        tok = strtok(NULL, " ");  //* get operator
        if (!tok || (strcmp(tok, ">") && strcmp(tok, "<"))) {
            fprintf(stderr,
                    "sbt:\e[31m syntax error:\e[39m %s: Conditional operator ('>' or '<') was expected\n",
                    tok);
            return ERROR_CODE;
        }
        str->root = ast_insert(str->root, priority_cnd_statement, tok, OPERATOR);
        tok = strtok(NULL, " ");
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The operand was expected\n", s);
            return ERROR_CODE;
        }
        str->root = ast_insert(str->root, priority_operand, tok, OPERAND);
        tok = strtok(NULL, " ");
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: Expected expression in the 'if' construct\n",
                    s);
            return ERROR_CODE;
        }
        build_ast(tok, str, vars, vars_size);
    } else if (!strcmp(s, "LET")) {
        str->root = ast_insert(str->root, priority_let, s, STATEMENT);
        char* tok = strtok(NULL, " ");
        if (!tok) {
            fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The operand was expected\n", s);
            return ERROR_CODE;
        }
        del_newline_in_str(tok);
        str->root = ast_insert(str->root, priority_operand, tok, OPERAND);
        variables_push_back(vars, *vars_size, tok, 0);
        if ((tok = strtok(NULL, " "))) {
            if (!strcmp(tok, "=")) {
                ast_insert(str->root, priority_cnd_statement, tok, OPERATOR);
                while ((tok = strtok(NULL, " "))) {
                    del_newline_in_str(tok);
                    if (!strcmp(tok, "+") || !strcmp(tok, "-")) {
                        ast_insert(str->root, priority_operator_low, tok, OPERATOR);
                    } else if (!strcmp(tok, "*") || !strcmp(tok, "/")) {
                        ast_insert(str->root, priority_operator_high, tok, OPERATOR);
                    } else if (is_all_uppercase(tok)) {
                        ast_insert(str->root, priority_operand, tok, OPERAND);
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
        // tok = strtok(NULL, " ");
        // if (tok)
        //     fprintf(stderr, "sbt:\e[31m syntax error:\e[39m %s: The end of the line was expected. Found:
        //     %s\n", s, tok);
    } else if (!strcmp(s, "END")) {
        str->root = ast_insert(str->root, priority_io, s, STATEMENT);
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

int sbt(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    sbstring strs[256];
    variables var[100];
    int var_size = 99;

    //? init variables array
    for (int i = 0; i < (int)(sizeof(var) / sizeof(var[0])); ++i)
        var[i].name = NULL, var[i].value = 0xffff, var[i].address = -1;

    if (!file) {
        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m %s: There is no such file or directory\n", filepath);
        return ERROR_CODE;
    }

    for (int i = 0; !feof(file); ++i) {
        char bf[BUFSIZ] = {0};
        fgets(bf, BUFSIZ, file);
        strs[i].var = var;
        strs[i].root = NULL;
        char* tok = strtok(bf, " ");  //* get instruction number
        strs[i].n = atoi(tok);
        if (strs[i].n == 0 && (strcmp(tok, "00") || strcmp(tok, "000"))) {
            fprintf(stderr, "sbt:\e[31m fatal error:\e[39m %s: Invalid instruction number\n", tok);
            return ERROR_CODE;
        }

        tok = strtok(NULL, " ");  //* get instruction
        if (build_ast(tok, &strs[i], var, &var_size)) {
            fprintf(stderr, "error line: %d\nexit code: 1\n", i);
            exit(1);
        }
    }

    // for (int i = 0; strs[i].root; ++i) {
    //     printf("str №%d:\n", i);
    //     ast_print(strs[i].root);
    //     printf("\n");
    // }

    fclose(file);
    for (int i = 0; strs[i].root; ++i) ast_free(strs[i].root);
    for (int i = 99; var[i].name && i >= 0; --i) free(var[i].name);

    return SUCCES_CODE;
}

int main(int argc, char** argv) {
    if (argc < 2) return ERROR_CODE;
    return sbt(argv[1]);
}

//* C = A - B
// root = ast_insert(root, 1, "C", OPERAND);
// root = ast_insert(root, 0, "=", ASSIGNMENT);
// root = ast_insert(root, 3, "A", OPERAND);
// root = ast_insert(root, 2, "-", OPERATOR);
// root = ast_insert(root, 3, "B", OPERAND);

// root = ast_insert(root, OPERAND, "C", OPERAND);
// root = ast_insert(root, ASSIGNMENT, "=", ASSIGNMENT);
// root = ast_insert(root, OPERAND, "A", OPERAND);
// root = ast_insert(root, OPERATOR, "-", OPERATOR);
// root = ast_insert(root, OPERAND, "B", OPERAND);

// ast_print(root);
// ast_free(root);

//* C = A + B * A
// root = ast_insert(root, 1, "C", OPERAND);
// root = ast_insert(root, 0, "=", ASSIGNMENT);
// root = ast_insert(root, 4, "A", OPERAND);
// root = ast_insert(root, 2, "+", OPERATOR);
// root = ast_insert(root, 4, "B", OPERAND);
// root = ast_insert(root, 3, "*", OPERATOR);
// root = ast_insert(root, 4, "A", OPERAND);

// ast_print(root);
// ast_free(root);

//* C = A + B * A - B / D
// root = ast_insert(root, 1, "C", OPERAND);
// root = ast_insert(root, 0, "=", OPERATOR);
// root = ast_insert(root, 4, "A", OPERAND);
// root = ast_insert(root, 2, "+", OPERATOR);
// root = ast_insert(root, 4, "B", OPERAND);
// root = ast_insert(root, 3, "*", OPERATOR);
// root = ast_insert(root, 4, "A", OPERAND);
// root = ast_insert(root, 2, "-", OPERATOR);
// root = ast_insert(root, 4, "B", OPERAND);
// root = ast_insert(root, 3, "/", OPERATOR);
// root = ast_insert(root, 4, "D", OPERAND);

//* GOTO

// root = ast_insert(root, 0, "IF", STATEMENT);
// root = ast_insert(root, 4, "C", OPERAND);
// root = ast_insert(root, 2, "<", OPERATOR);
// root = ast_insert(root, 4, "0", OPERAND);
// root = ast_insert(root, 1, "GOTO", STATEMENT);
// root = ast_insert(root, 4, "20", OPERAND);
