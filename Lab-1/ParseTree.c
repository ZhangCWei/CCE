#include "ParseTree.h"
#include <stdio.h>
#include <stdlib.h>

TreeNode* createTreeNode(char* node_name, NodeType node_type, int line_number, int num_children, TreeNode** children) {
    // 创建新节点，申请内存
    TreeNode* new_node = (TreeNode*)malloc(sizeof(TreeNode));
    if (new_node == NULL) {
        fprintf(stderr, "TreeNode memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    // 新节点初始化
    *new_node = (TreeNode){
        .node_name = node_name,
        .node_type = node_type,
        .line_number = line_number,
        .num_children = num_children,
        .children = children,
    };

    // 检查是否所有的子节点为NULL
    int all_children_null = 1;
    for (int i = 0; i < new_node->num_children; i++) {
        if (new_node->children[i]->node_type != NODE_TYPE_NULL) {
            all_children_null = 0;
            break;
        }
    }

    // 若所有子节点为NULL，则更新节点类型为NULL
    if (node_type == NODE_TYPE_NOT_NULL && all_children_null) {
        new_node->node_type = NODE_TYPE_NULL;
    }

    return new_node;
}

void printTree(TreeNode* root, int depth) {
    // 打印空格
    if (root->node_type != NODE_TYPE_NULL) {
        for (int i = 0; i < depth; i++) {
            printf("  ");
        }
    }

    // 打印节点
    switch (root->node_type) {
        case NODE_TYPE_NOT_NULL:
            printf("%s (%d)\n", root->node_name, root->line_number);
            break;
        case NODE_TYPE_NULL:
            break;
        case NODE_TYPE_IDENTIFIER:
            printf("%s: %s\n", root->node_name, root->str_value);
            break;
        case NODE_TYPE_INTEGER:
            printf("%s: %d\n", root->node_name, root->int_value);
            break;
        case NODE_TYPE_FLOAT:
            printf("%s: %f\n", root->node_name, root->float_value);
            break;
        case NODE_TYPE_TYPE:
            printf("%s: %s\n", root->node_name, root->str_value);
            break;
        case NODE_TYPE_OTHER:
            printf("%s\n", root->node_name);
            break;
    }

    //递归地打印下一层节点
    for (int i = 0; i < root->num_children; i++) {
        printTree(root->children[i], depth + 1);
    }
}

