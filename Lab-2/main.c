#include <stdio.h>
#include "ParseTree.h"
#include "semantic.h"

// 声明外部函数和变量
extern int yyrestart(FILE* f);                      // 重新设置词法分析的输入源
extern int yyparse();                               // 执行语法分析
extern void printTree(TreeNode* root, int depth);   // 打印语法树
extern TreeNode* root;                              // 语法树的根节点
extern int lexicalErrorCount;                       // 词法错误计数
extern int syntaxErrorCount;                        // 语法错误计数

int main(int argc, char** argv) {
    if (argc <= 1)
        return 1;

    // 打开输入文件
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        // 输出文件打开失败的错误信息
        perror(argv[1]);
        return 1;
    }

    // 设置词法分析的输入源为打开的文件
    yyrestart(f);
    
    // 执行语法分析(其中包括词法分析)
    yyparse();

    // 如果语法树非空且没有词法错误和语法错误，则打印语法树
    if (root != NULL && lexicalErrorCount == 0 && syntaxErrorCount == 0)
        semantic_analyse(root);
    
    return 0;
}
