#include <sc/helper.h>
#include <sc/ram-operations.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct abstract_syntax_tree ast_t;

struct abstract_syntax_tree {
    ast_t* left;
    ast_t* right;
    char* data;
    int priority;
    int node_type;
};

enum ast_types { ASSIGNMENT, OPERATOR, OPERAND };

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

ast_t* _ast_insert(ast_t* root, int priority, char* data, int node_type) {
    if (!root) {
        root = ast_create(priority, data, node_type);
    } else if (root->priority < priority) {
        root->right = _ast_insert(root->right, priority, data, node_type);
    } else {
        ast_t* node = ast_create(priority, data, node_type);
        node->left = root;
        return node;
    }
    return root;
}

ast_t* ast_insert(ast_t* root, int priority, char* data, int node_type) {
    return _ast_insert(root, priority, data, node_type);
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
    if (tree == NULL) return;

    printf("[%d]%s", tree->priority, tree->data);
    printf("\n");

    if (tree->left != NULL) {
        __ast_print("|\n", p, s);
        __ast_print("L: ", p, s);
        _ast_print(tree->left, p + 1, s + ((tree->right == NULL ? 0 : 1) << p));
    }
    if (tree->right != NULL) {
        __ast_print("|\n", p, s);
        __ast_print("R: ", p, s);
        _ast_print(tree->right, p + 1, s);
    }
}

void ast_print(ast_t* tree) { _ast_print(tree, 0, 0); }

int sbt(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "sbt:\e[31m fatal error:\e[39m %s: There is no such file or directory\n", filepath);
        return ERROR_CODE;
    }
    fclose(file);

    ast_t* root = NULL;

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
    root = ast_insert(root, 1, "C", OPERAND);
    root = ast_insert(root, 0, "=", ASSIGNMENT);
    root = ast_insert(root, 4, "A", OPERAND);
    root = ast_insert(root, 2, "+", OPERATOR);
    root = ast_insert(root, 4, "B", OPERAND);
    root = ast_insert(root, 3, "*", OPERATOR);
    root = ast_insert(root, 4, "A", OPERAND);
    root = ast_insert(root, 2, "-", OPERATOR);
    root = ast_insert(root, 4, "B", OPERAND);
    root = ast_insert(root, 3, "/", OPERATOR);
    root = ast_insert(root, 4, "D", OPERAND);

    ast_print(root);
    ast_free(root);

    return SUCCES_CODE;
}

int main(int argc, char** argv) {
    if (argc < 2) return ERROR_CODE;
    return sbt(argv[1]);
}
