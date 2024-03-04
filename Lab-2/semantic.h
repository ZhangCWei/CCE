#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdio.h>
#include <stdlib.h>
#include "ParseTree.h"

// 两个基本类型
#define INT_TYPE 0
#define FLOAT_TYPE 1

// 哈希表大小
#define HASH_SIZE 1024

// 定义了一些自定义数据结构和类型别名
typedef struct Type_d Type_;
typedef Type_* Type;
typedef struct FieldList_d FieldList_;
typedef FieldList_* FieldList;
typedef struct Structure_d Structure_;
typedef Structure_* Structure;
typedef struct Function_d Function_;
typedef Function_* Function;
typedef struct Entry_d Entry_;
typedef Entry_* Entry;

// 表示不同的数据类型种类
typedef enum {
    ENUM_BASIC,        // 基本类型
    ENUM_ARRAY,        // 数组类型
    ENUM_STRUCT,       // 结构体类型
    ENUM_STRUCT_DEF,   // 结构体定义
    ENUM_FUNC          // 函数类型
} Kind;

// 表示标识符的种类
typedef enum {
    ENUM_VAR,   // 变量
    ENUM_FIELD  // 结构体中的域
} IdType;

// 数据类型结构
struct Type_d {
    Kind kind;
    union {
        int basic;              // 基本类型
        struct {
            Type elem;          // 数组元素的类型
            int size;           // 数组大小
        } array;                // 数组类型
        Structure structure;    // 结构体类型
        Function func;          // 函数类型
    };
};

// 结构体域链表节点
struct FieldList_d {
    char node_name[32];  // 域的名字
    Type type;      // 域的类型
    FieldList next; // 下一个域的指针
};

// 结构体类型
struct Structure_d {
    char node_name[32];    // 结构体的名字
    FieldList head;   // 结构体域链表头
};

// 函数类型
struct Function_d {
    char node_name[32];     // 函数名
    Type returnType;   // 返回值类型
    int parmNum;       // 参数个数
    FieldList head;    // 参数链表头节点指针
    int hasDefined;    // 是否已经定义
    int line_number;        // 所在行数
};

// 符号表条目类型
struct Entry_d {
    char node_name[32];    // 条目的名字
    Type type;        // 条目的类型
    Entry hashNext;   // 指向同一槽位的下一个条目
    Entry layerNext;  // 指向同一层次的下一个条目
};

// 函数声明部分

// 进行语义分析，初始化符号表，并调用Program分析AST根节点
void semantic_analyse(TreeNode* root);
// 解析程序的入口点，调用ExtDefList处理外部定义列表
void Program(TreeNode* root);
// 检查是否存在仅有声明而没有实现的函数
void check();
// 处理外部定义列表，递归调用ExtDef处理每个外部定义
void ExtDefList(TreeNode* root);
// 处理外部定义，包括全局变量定义和函数定义
void ExtDef(TreeNode* root);
// 解析类型描述符，返回Type
Type Specifier(TreeNode* root);
// 处理全局变量名列表
void ExtDecList(TreeNode* root, Type type);
// 处理函数名和参数列表，返回Function
Function FunDec(TreeNode* root);
// 处理复合语句，包括函数参数的处理
void CompSt(TreeNode* root, char* funcName, Type reType);
// 解析结构体定义或使用，返回Type
Type StructSpecifier(TreeNode* root);
// 处理变量定义列表，包括全局变量和结构体域定义
FieldList DefList(TreeNode* root, IdType class);
// 处理变量定义，返回FieldList
FieldList Def(TreeNode* root, IdType class);
// 处理声明列表，包括变量声明
FieldList DecList(TreeNode* root, Type type, IdType class);
// 处理单个声明，返回FieldList
FieldList Dec(TreeNode* root, Type type, IdType class);
// 处理变量声明，包括数组和普通变量
FieldList VarDec(TreeNode* root, Type type, IdType class);
// 处理参数列表，返回FieldList
FieldList VarList(TreeNode* root);
// 处理函数参数声明，返回FieldList
FieldList ParamDec(TreeNode* root);
// 处理语句列表
void StmtList(TreeNode* root, Type retype);
// 处理单个语句
void Stmt(TreeNode* root, Type reType);
// 处理表达式，进行类型检查，返回Type
Type Exp(TreeNode* root);
// 处理函数调用参数列表，返回参数的FieldList
FieldList Args(TreeNode* root);
// 打印参数类型列表
void printArgs(FieldList head);
// 打印类型的信息
void printType(Type type);


#endif
