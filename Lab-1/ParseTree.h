#ifndef TREE_H
#define TREE_H

typedef enum {
    NODE_TYPE_NOT_NULL = 1,
    NODE_TYPE_NULL,
    NODE_TYPE_IDENTIFIER,
    NODE_TYPE_INTEGER,
    NODE_TYPE_FLOAT,
    NODE_TYPE_TYPE,
    NODE_TYPE_OTHER
} NodeType;

typedef struct TreeNode {
    char* node_name;
    NodeType node_type;
    int line_number;
    int num_children;
    union {
        char str_value[32];
        int int_value;
        float float_value;
    };
    struct TreeNode** children;
} TreeNode;

TreeNode* createTreeNode(char* node_name, NodeType node_type, int line_number, int num_children, TreeNode** children);
void printTree(TreeNode* root, int depth);

#endif /* TREE_H */

