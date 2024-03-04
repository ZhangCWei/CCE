#include "semantic.h"
#include <string.h>

// 用散列表实现符号表
Entry symbolTable[HASH_SIZE];  // 散列表数组，用于存储符号表条目
Entry layersHead;  // 符号表中不同层次的链表头节点

// 初始化符号表
void initSymbolTable() {
    for (int i = 0; i < HASH_SIZE; i++) {
        // 散列表的每个槽位都初始化为空指针
        symbolTable[i] = NULL;
    }
    // 初始化层次链表头节点
    layersHead = (Entry)malloc(sizeof(Entry_));
    layersHead->hashNext = NULL;
    layersHead->layerNext = NULL;
    // 初始化全局层次节点
    Entry globalLayer = (Entry)malloc(sizeof(Entry_));
    globalLayer->hashNext = NULL;
    globalLayer->layerNext = NULL;
    layersHead->hashNext = globalLayer;
}

// 散列函数
unsigned int hash_pjw(char* node_name) {
    unsigned int val = 0, i;
    for (; *node_name; ++node_name) {
        val = (val << 2) + *node_name;  // 计算散列值
        if (i = val & ~HASH_SIZE) {
            val = (val ^ (i >> 12)) & HASH_SIZE;
            // 如果散列值超出表大小，进行一些位运算调整
        }
    }
    return val;  // 返回散列后的值
}

// 向符号表中插入符号
void insertSymbol(Entry symbol) {
    // 计算散列值
    unsigned int hash = hash_pjw(symbol->node_name);
    // 插入对应槽位的链表头
    Entry tail = symbolTable[hash];
    symbolTable[hash] = symbol;
    symbol->hashNext = tail;
    // 插入对应层次的链表头
    Entry currentLayer = layersHead->hashNext;
    tail = currentLayer->layerNext;
    currentLayer->layerNext = symbol;
    symbol->layerNext = tail;
}

// 从符号表中查找符号
Entry findSymbolAll(char* node_name) {
    unsigned int hash = hash_pjw(node_name); // 使用散列函数计算符号的哈希值
    Entry tmp = symbolTable[hash];      // 获取哈希槽位中的第一个条目

    while (tmp != NULL) {
        if (strcmp(tmp->node_name, node_name) == 0 && tmp->type != NULL && tmp->type->kind != ENUM_FUNC) {
            // 比较当前条目的名字和目标名字，以及类型不是函数类型
            // 注意：这里通过比较名字和类型来确定是否找到符号，且不是函数类型
            break;
        }
        tmp = tmp->hashNext; // 移动到同一槽位的下一个条目
    }
    
    return tmp; // 返回找到的符号的条目（或者 NULL，如果未找到）
}

// 查找同一层次的符号
Entry findSymbolLayer(char* node_name) {
    Entry currentLayer = layersHead->hashNext;  // 获取当前层次的第一个符号条目
    Entry symbol = currentLayer->layerNext;     // 获取当前层次的下一个符号条目

    while (symbol != NULL) {
        if (strcmp(symbol->node_name, node_name) == 0 && symbol->type->kind != ENUM_FUNC) {
            // 比较当前符号的名字和目标名字，以及类型不是函数类型
            // 注意：这里通过比较名字和类型来确定是否找到符号，且不是函数类型
            break;
        }
        symbol = symbol->layerNext; // 移动到同一层次的下一个符号条目
    }

    return symbol; // 返回找到的符号的条目（或者 NULL，如果未找到）
}


// 查找函数
Entry findSymbolFunc(char* node_name) {
    unsigned int hash = hash_pjw(node_name); // 使用散列函数计算函数名的哈希值
    Entry tmp = symbolTable[hash];      // 获取哈希槽位中的第一个条目

    while (tmp != NULL) {
        if (strcmp(tmp->node_name, node_name) == 0 && tmp->type->kind == ENUM_FUNC) {
            // 比较当前条目的名字和目标名字，以及类型为函数类型
            // 注意：这里通过比较名字和类型来确定是否找到函数符号
            break;
        }
        tmp = tmp->hashNext; // 移动到同一槽位的下一个条目
    }

    return tmp; // 返回找到的函数符号的条目（或者 NULL，如果未找到）
}


// 从符号表中删除符号
void delSymbol(char* node_name) {
    unsigned int hash = hash_pjw(node_name); // 使用散列函数计算符号的哈希值
    Entry pre = NULL;                   // 用于跟踪前一个条目
    Entry tmp = symbolTable[hash];      // 获取哈希槽位中的第一个条目

    while (tmp != NULL) {
        if (strcmp(tmp->node_name, node_name) == 0) {
            // 找到了匹配的符号节点
            break;
        }
        pre = tmp;              // 将当前条目设为前一个条目
        tmp = tmp->hashNext;    // 移动到同一槽位的下一个条目
    }

    // 说明要删除的符号节点是链表的头节点
    if (tmp != NULL && pre == NULL) {
        symbolTable[hash] = tmp->hashNext;  // 更新哈希槽位的头节点
    }
    else if (tmp != NULL && pre != NULL) {
        pre->hashNext = tmp->hashNext;      // 更新前一个节点的 next 指针
    }
}


// 插入一个新的层次
void pushLayer() {
    // 创建一个新的层次节点
    Entry currentLayer = (Entry)malloc(sizeof(Entry_));
    currentLayer->hashNext = NULL;
    currentLayer->layerNext = NULL;

    // 将新层次节点插入到链表中
    Entry tail = layersHead->hashNext;      // 获取当前最外层的节点
    layersHead->hashNext = currentLayer;    // 设置新节点为最外层
    currentLayer->hashNext = tail;          // 新节点的下一个是之前的最外层
}


// 弹出一个层次，同时删除该层次对应的所有符号
void popLayer() {
    Entry currentLayer = layersHead->hashNext;      // 获取当前最外层的节点
    layersHead->hashNext = currentLayer->hashNext;  // 移除当前最外层

    // 删除该层次中的所有符号
    Entry symbol = currentLayer->layerNext; // 获取层次内的第一个符号
    while (symbol != NULL) {
        delSymbol(symbol->node_name);    // 删除符号
        symbol = symbol->layerNext; // 移动到下一个符号
    }
}


// 类型等价判断函数
int typeEqual(Type a, Type b) {
    if (a == NULL && b == NULL)
        return 1; // 如果两个类型都是 NULL，它们是等价的
    else if (a == NULL || b == NULL)
        return 0; // 如果一个类型为 NULL，一个不为 NULL，它们不等价
    else if (a->kind != b->kind)
        return 0; // 如果类型的种类不同，它们不等价
    else if (a->kind == ENUM_BASIC) {
        return a->basic == b->basic; // 如果是基本类型，名等价需要基本类型相同
    }
    // 数组等价——基类型和维数相同
    else if (a->kind == ENUM_ARRAY) {
        return typeEqual(a->array.elem, b->array.elem); // 数组等价需要基类型和维数相同
    }
    // 结构体类型结构等价
    else if (a->kind == ENUM_STRUCT) {
        FieldList aFields = a->structure->head;
        FieldList bFields = b->structure->head;
        int res = 1;
        while (aFields != NULL && bFields != NULL) {
            if (!typeEqual(aFields->type, bFields->type)) {
                res = 0; // 如果结构体字段类型不等价，它们不等价
                break;
            }
            aFields = aFields->next;
            bFields = bFields->next;
        }
        if (aFields != NULL || bFields != NULL)
            res = 0; // 如果一个结构体字段链表较长，它们不等价
        return res;
    }
    // 函数类型等价——返回类型、参数个数和参数类型等价
    else if (a->kind == ENUM_FUNC) {
        if (!typeEqual(a->func->returnType, b->func->returnType))
            return 0; // 如果返回类型不等价，它们不等价
        if (a->func->parmNum != b->func->parmNum)
            return 0; // 如果参数个数不等，它们不等价
        FieldList aFields = a->func->head;
        FieldList bFields = b->func->head;
        int res = 1;
        while (aFields != NULL && bFields != NULL) {
            if (!typeEqual(aFields->type, bFields->type)) {
                res = 0; // 如果参数类型不等价，它们不等价
                break;
            }
            aFields = aFields->next;
            bFields = bFields->next;
        }
        if (aFields != NULL || bFields != NULL)
            res = 0; // 如果一个函数参数链表较长，它们不等价
        return res;
    }
    else
        return 0; // 其他情况下不等价
}


// 开始进行语法分析树的语义解析
void semantic_analyse(TreeNode* root) {
    initSymbolTable();  // 初始化符号表
    Program(root);      // 调用 Program 函数解析程序的语义
    check();            // 检查是否存在仅有声明但没有实现的函数
}

// Program 函数用于解析程序的语义
void Program(TreeNode* root) {
    ExtDefList(root->children[0]); // 调用 ExtDefList 函数解析程序中的外部定义部分
}


// 检查是否存在函数声明但没有实现的情况
void check() {
    for (int i = 0; i < HASH_SIZE; i++) {
        if (symbolTable[i] != NULL) {
            Entry entry = symbolTable[i]; // 获取当前哈希槽位中的第一个条目
            while (entry != NULL) {
                if (entry->type->kind == ENUM_FUNC && entry->type->func->hasDefined == 0) {
                    // 如果是函数类型并且没有定义实现
                    printf("Error type 18 at line %d: Undefined function \"%s\".\n", entry->type->func->line_number, entry->node_name);
                    // 输出错误信息，表示未定义的函数
                }
                entry = entry->hashNext; // 移动到同一槽位的下一个条目
            }
        }
    }
}

// 解析外部定义列表
void ExtDefList(TreeNode* root) {
    if (root->num_children != 0) {
        ExtDef(root->children[0]);      // 解析第一个外部定义
        ExtDefList(root->children[1]);  // 递归解析剩余的外部定义
    }
}


void ExtDef(TreeNode* root) {
    Type type = Specifier(root->children[0]); // 获取外部定义的类型

    // 类型错误（结构体重复定义）
    if (type == NULL)
        return;

    // 结构体定义，是结构体，不是匿名类型，域定义没有产生错误
    if (type->kind == ENUM_STRUCT && type->structure->node_name != "" && type->structure->head != NULL) {
        // 创建新的符号表条目以表示结构体定义
        Entry res = (Entry)malloc(sizeof(Entry_));
        strcpy(res->node_name, type->structure->node_name);
        res->type = (Type)malloc(sizeof(Type_));
        res->type->structure = type->structure;
        res->type->kind = ENUM_STRUCT_DEF;
        insertSymbol(res); // 插入符号表中
    }

    // 全局变量定义
    if (strcmp(root->children[1]->node_name, "ExtDecList") == 0) {
        ExtDecList(root->children[1], type);
    }

    // 函数定义
    if (strcmp(root->children[1]->node_name, "FunDec") == 0) {
        Function func = FunDec(root->children[1]);
        func->returnType = type;
        func->hasDefined = 0;
        Type newType = (Type)malloc(sizeof(Type_));
        newType->kind = ENUM_FUNC;
        newType->func = func;

        // 在符号表中查找是否已经存在同名函数声明或定义
        Entry sym = findSymbolFunc(func->node_name);

        // 存在同名函数声明/定义
        if (sym != NULL) {
            // 是函数定义
            if (sym->type->func->hasDefined == 1) {
                // 重复定义
                if (strcmp(root->children[2]->node_name, "CompSt") == 0)
                    printf("Error type 4 at line %d: Redefined function \"%s\".\n", root->line_number, sym->node_name);
                // 声明和定义冲突
                else if (strcmp(root->children[2]->node_name, "SEMI") == 0 && !typeEqual(newType, sym->type))
                    printf("Error type 19 at line %d: Inconsistent declaration of function \"%s\".\n", root->line_number, sym->node_name);
                return;
            }

            // 是函数声明
            if (sym->type->func->hasDefined == 0) {
                if (strcmp(root->children[2]->node_name, "CompSt") == 0) {
                    // 定义和声明冲突
                    if (!typeEqual(newType, sym->type)) {
                        printf("Error type 19 at line %d: Inconsistent declaration of function \"%s\".\n", root->line_number, sym->node_name);
                        return;
                    }
                    // 为已声明的函数添加定义
                    else {
                        sym->type->func->hasDefined = 1;
                        return;
                    }
                }
                else {
                    // 声明和声明冲突
                    if (!typeEqual(newType, sym->type))
                        printf("Error type 19 at line %d: Inconsistent declaration of function \"%s\".\n", root->line_number, sym->node_name);
                    return;
                }
            }
        }

        // 是首次出现的函数声明/定义
        if (strcmp(root->children[2]->node_name, "SEMI") == 0) {
            Entry res = (Entry)malloc(sizeof(Entry_));
            strcpy(res->node_name, func->node_name);
            res->type = newType;
            insertSymbol(res);
        }
        else {
            Entry res = (Entry)malloc(sizeof(Entry_));
            func->hasDefined = 1;
            strcpy(res->node_name, func->node_name);
            res->type = newType;
            insertSymbol(res);
            pushLayer(); // 插入新的符号表层次
            CompSt(root->children[2], func->node_name, func->returnType); // 解析函数体
            popLayer(); // 弹出符号表层次
        }
        return;
    }
    return;
}


// 解析类型描述符
Type Specifier(TreeNode* root) {
    root = root->children[0]; // 获取子节点，用于判断类型描述符的类型

    if (strcmp(root->node_name, "TYPE") == 0) {
        // 如果是基本类型(int 或 float)
        Type res = (Type)malloc(sizeof(Type_));
        res->kind = ENUM_BASIC;
        if (strcmp(root->str_value, "int") == 0)
            res->basic = INT_TYPE;
        else if (strcmp(root->str_value, "float") == 0)
            res->basic = FLOAT_TYPE;
        return res;
    }
    else if (strcmp(root->node_name, "StructSpecifier") == 0) {
        // 如果是结构体类型描述符，则调用 StructSpecifier 函数来解析
        return StructSpecifier(root);
    }
}


// 处理全局变量声明列表
void ExtDecList(TreeNode* root, Type type) {
    // 如果只有一个变量声明，它调用 VarDec 函数处理
    if (root->num_children == 1)
        VarDec(root->children[0], type, ENUM_VAR);
    // 如果有多个变量声明，递归调用自身来处理剩余的声明
    else {
        VarDec(root->children[0], type, ENUM_VAR);
        ExtDecList(root->children[2], type);
    }
}


// 处理函数声明，包括函数名和参数列表(不检查错误)
Function FunDec(TreeNode* root) {
    Function res = (Function)malloc(sizeof(Function_));
    strcpy(res->node_name, root->children[0]->str_value);
    res->parmNum = 0;
    res->line_number = root->line_number;

    // 如果函数没有参数
    if (root->num_children == 3)
        res->head = NULL;
    else {
        pushLayer(); // 插入新的符号表层次
        res->head = VarList(root->children[2]); // 解析参数列表
        popLayer(); // 弹出符号表层次

        // 统计参数个数
        FieldList tmp = res->head;
        while (tmp != NULL) {
            res->parmNum += 1;
            tmp = tmp->next;
        }
    }
    return res;
}

// 处理函数体的语义分析
void CompSt(TreeNode* root, char* funcName, Type reType) {
    // 如果有函数名，将函数参数存进符号表
    Type type = reType;
    if (funcName != NULL) {
        // 查找函数名对应的符号
        Entry sym = findSymbolFunc(funcName);
        FieldList parms = sym->type->func->head;
        while (parms != NULL) {
            // 创建参数的符号表条目
            Entry parm = (Entry)malloc(sizeof(Entry_));
            strcpy(parm->node_name, parms->node_name);
            parm->type = parms->type;
            // 将参数插入符号表
            insertSymbol(parm);
            parms = parms->next;
        }
        type = sym->type->func->returnType; // 更新当前作用域的返回值类型
    }

    // 解析局部变量的定义
    DefList(root->children[1], ENUM_VAR);

    // 解析函数体内的语句，使用更新后的返回值类型
    StmtList(root->children[2], type);

    return;
}


Type StructSpecifier(TreeNode* root) {
    Type res = (Type)malloc(sizeof(Type_));
    res->kind = ENUM_STRUCT;
    res->structure = (Structure)malloc(sizeof(Structure_));

    for (int i = 0; i < root->num_children; i++) {
        TreeNode* child = root->children[i];

        if (strcmp(child->node_name, "OptTag") == 0) {
            if (child->num_children == 0)
                strcpy(res->structure->node_name, ""); // 结构体没有指定名字
            else {
                // 结构体名字需要全局唯一
                Entry sym = findSymbolAll(child->children[0]->str_value);
                // 检查结构体的名字是否与已定义的结构体或变量的名字重复
                if (sym != NULL) {
                    printf("Error type 16 at line %d: Duplicated node_name \"%s\".\n", child->line_number, child->children[0]->str_value);
                    return NULL;
                }
                strcpy(res->structure->node_name, child->children[0]->str_value);
            }
        }
        // 在这里，要使用一个已经定义的结构体类型，可能会产生错误
        else if (strcmp(child->node_name, "Tag") == 0) {
            Entry sym = findSymbolAll(child->children[0]->str_value);
            // 如果结构体的名称不在符号表中，或查找到的条目不属于结构体定义类型，报错
            if (sym == NULL || sym->type->kind != ENUM_STRUCT_DEF) {
                printf("Error type 17 at line %d: Undefined struct \"%s\".\n", child->line_number, child->children[0]->str_value);
                return NULL;
            }
            res->structure = sym->type->structure; // 使用已定义的结构体
            return res;
        }
        else if (strcmp(child->node_name, "DefList") == 0) {
            pushLayer(); // 插入新的符号表层次，用于处理结构体域的定义
            res->structure->head = DefList(child, ENUM_FIELD); // 解析结构体的域
            popLayer(); // 弹出符号表层次
            return res;
        }
    }
}


// 变量定义
FieldList DefList(TreeNode* root, IdType class) {
    if (root->num_children == 0)
        return NULL; // 如果没有变量定义，返回空指针
    else {
        FieldList res = Def(root->children[0], class); // 处理第一个变量定义
        if (res == NULL)
            res = DefList(root->children[1], class); // 递归处理剩余的变量定义
        else {
            // 将新的变量定义挂在已有变量定义的末尾
            FieldList tmp = res;
            while (tmp->next != NULL) tmp = tmp->next;
            tmp->next = DefList(root->children[1], class);
        }
        return res;
    }
}

// 用于处理单个变量的定义
FieldList Def(TreeNode* root, IdType class) {
    Type type = Specifier(root->children[0]); // 获取变量类型
    FieldList res = DecList(root->children[1], type, class); // 处理变量列表
    return res;
}

// 用于处理变量列表
FieldList DecList(TreeNode* root, Type type, IdType class) {
    FieldList res = Dec(root->children[0], type, class); // 处理单个变量
    if (root->num_children == 3) {
        if (res == NULL)
            res = DecList(root->children[2], type, class); // 递归处理剩余的变量
        else {
            FieldList tmp = res;
            while (tmp->next != NULL) tmp = tmp->next;
            tmp->next = DecList(root->children[2], type, class);
        }
    }
    return res;
}


FieldList Dec(TreeNode* root, Type type, IdType class) {
    FieldList res = VarDec(root->children[0], type, class); // 处理变量名和类型
    // 错误：在定义结构体时对域进行初始化
    if (class == ENUM_FIELD && res != NULL && root->num_children == 3) {
        printf("Error type 15 at line %d: Initialized field \"%s\".\n", root->line_number, res->node_name);
        return NULL;
    }
    if (class == ENUM_VAR && res != NULL && root->num_children == 3) {
        Type right = Exp(root->children[2]); // 获取初始化表达式的类型
        //错误：初始化表达式的类型与变量的类型不匹配
        if (right != NULL && !typeEqual(right, type)) {
            printf("Error type 5 at line %d: Type mismatched.\n", root->line_number);
            return NULL;
        }
    }
    return res;
}

// 处理变量名、类型以及可能的数组定义
FieldList VarDec(TreeNode* root, Type type, IdType class) {
    // 只有一个子节点，表示变量是普通变量
    if (root->num_children == 1) {
        Entry sym = findSymbolLayer(root->children[0]->str_value);
        Entry symA = findSymbolAll(root->children[0]->str_value);
        // 域名或变量名重复定义或与结构体定义重复
        if (sym != NULL || (symA != NULL && symA->type->kind == ENUM_STRUCT_DEF)) {
            if (class == ENUM_FIELD)
                printf("Error type 15 at line %d: Redefined field \"%s\".\n", root->line_number, root->children[0]->str_value);
            if (class == ENUM_VAR)
                printf("Error type 3 at line %d: Redefined variable \"%s\".\n", root->line_number, root->children[0]->str_value);
            return NULL;
        }
        FieldList res = (FieldList)malloc(sizeof(FieldList_));
        strcpy(res->node_name, root->children[0]->str_value);
        res->type = type;
        // 域也要加符号表
        Entry tmp = (Entry)malloc(sizeof(Entry_));
        strcpy(tmp->node_name, root->children[0]->str_value);
        tmp->type = type;
        insertSymbol(tmp); 
        return res;
    }
    // 数组
    else {
        // 函数创建一个新的类型，将其类型标记为数组类型
        Type newType = (Type)malloc(sizeof(Type_));
        newType->kind = ENUM_ARRAY;
        newType->array.elem = type;
        newType->array.size = root->children[2]->int_value;
        // 处理可能的嵌套数组定义
        return VarDec(root->children[0], newType, class);
    }
}

// 参数列表
FieldList VarList(TreeNode* root) {
    FieldList res = ParamDec(root->children[0]); // 处理第一个参数
    if (root->num_children == 3) {
        res->next = VarList(root->children[2]); // 递归处理剩余的参数
    }
    return res;
}

// 函数参数
FieldList ParamDec(TreeNode* root) {
    Type type = Specifier(root->children[0]);
    // 递归处理函数参数
    return VarDec(root->children[1], type, ENUM_FIELD);
}

// 语句列表
void StmtList(TreeNode* root, Type reType) {
    if (root->num_children == 2) {
        Stmt(root->children[0], reType);
        StmtList(root->children[1], reType);
    }
    return;
}


// 处理不同类型的语句，包括返回语句、复合语句、表达式语句、循环语句和条件语句
void Stmt(TreeNode* root, Type reType) {
    if (strcmp(root->children[0]->node_name, "RETURN") == 0) {
        Type type = Exp(root->children[1]);         // 处理返回语句的表达式
        if (!typeEqual(reType, type))
            printf("Error type 8 at line %d: Type mismatched for return.\n", root->line_number); // 返回值类型与函数返回类型不匹配
    }
    else if (strcmp(root->children[0]->node_name, "CompSt") == 0) {
        pushLayer();                                // 进入一个新的作用域
        CompSt(root->children[0], NULL, reType);    // 处理复合语句
        popLayer();                                 // 离开当前作用域
    }
    else if (strcmp(root->children[0]->node_name, "Exp") == 0) {
        Exp(root->children[0]);                     // 处理表达式语句
    }
    else if (strcmp(root->children[0]->node_name, "WHILE") == 0) {
        Exp(root->children[2]);                     // 处理循环条件表达式
        Stmt(root->children[4], reType);            // 处理循环体语句
    }
    else if (strcmp(root->children[0]->node_name, "IF") == 0) {
        Exp(root->children[2]);                     // 处理条件表达式
        Stmt(root->children[4], reType);            // 处理条件为真时的语句
        if (root->num_children == 7)
            Stmt(root->children[6], reType);        // 处理条件为假时的语句
    }
    return;
}


Type Exp(TreeNode* root) {
    if (strcmp(root->children[0]->node_name, "Exp") == 0) {
        // 对结构体使用.操作符
        if (strcmp(root->children[1]->node_name, "DOT") == 0) {
            Type res = Exp(root->children[0]);
            if (res != NULL) {
                if (res->kind != ENUM_STRUCT) {
                    printf("Error type 13 at line %d: Illegal use of \".\".\n", root->line_number);
                    return NULL;
                }
                char field[32];
                strcpy(field, root->children[2]->str_value);
                // 检测域名是否有效
                FieldList head = res->structure->head;
                Type ans = NULL;
                while (head != NULL) {
                    if (strcmp(field, head->node_name) == 0) {
                        ans = head->type;
                        break;
                    }
                    head = head->next;
                }
                // 域名不存在
                if (ans == NULL) {
                    printf("Error type 14 at line %d: Non-existed field \"%s\".\n", root->line_number, field);
                    return NULL;
                }
                return ans;
            }
            return res;
        }
        // 数组取地址操作
        else if (strcmp(root->children[1]->node_name, "LB") == 0) {
            Type pre = Exp(root->children[0]);
            if (pre != NULL) {
                if (pre->kind != ENUM_ARRAY) {
                    printf("Error type 10 at line %d: Expression is not an array.\n", root->line_number);
                    return NULL;
                }
                Type index = Exp(root->children[2]);
                if (index == NULL || index->kind != ENUM_BASIC || index->basic != INT_TYPE) {
                    printf("Error type 12 at line %d: Array index must be an integer.\n", root->line_number);
				    return NULL;
                }
                return pre->array.elem;
            }
            return pre;
        }
        // 赋值操作
        else if (strcmp(root->children[1]->node_name, "ASSIGNOP") == 0) {
            // 左值只有三种情况
            // 1、变量
            // 2、域
            // 3、数组元素
            TreeNode* left = root->children[0];
            TreeNode* right = root->children[2];
            Type leftType = NULL;
            Type rightType = Exp(right);
            if ((left->num_children == 1 && strcmp(left->children[0]->node_name, "ID") == 0) ||
				(left->num_children == 4 && strcmp(left->children[1]->node_name, "LB") == 0) ||
			    (left->num_children == 3 && strcmp(left->children[1]->node_name, "DOT") == 0))
				leftType = Exp(left);
			else {
				printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable.\n", root->line_number);
                return NULL;
            }
            if (leftType != NULL && rightType != NULL && !typeEqual(leftType, rightType)) {
                printf("Error type 5 at line %d: Type mismatched for assignment.\n", root->line_number);
                return NULL;
            }
            return leftType;
        }
        // 普通二元运算操作
        else {
            Type pre = Exp(root->children[0]);
            Type aft = Exp(root->children[2]);
            if (pre == NULL || aft == NULL)
                return NULL;
            if (!typeEqual(pre, aft)) {
                printf("Error type 7 at line %d: Type mismatched for operands.\n", root->line_number);
				return NULL;
            }
            if (strcmp(root->children[1]->node_name, "RELOP") == 0) {
                Type res = (Type)malloc(sizeof(Type_));
                res->kind = ENUM_BASIC;
                res->basic = INT_TYPE;
                return res;  
            }
            return pre;
        }
    }
    else if (strcmp(root->children[0]->node_name, "LP") == 0) {
        Type res = Exp(root->children[1]);
        return res;
    }
    else if (strcmp(root->children[0]->node_name, "MINUS") == 0) {
        Type res = Exp(root->children[1]);
        // 如果res为NULL应该是Exp有错，这里就不再报连锁错误
        if (res != NULL)
            if (res->kind != ENUM_BASIC) {
                printf("Error type 7 at line %d: Operands type mismatched.\n", root->line_number);
                return NULL;
            }
        return res;
    }
    else if (strcmp(root->children[0]->node_name, "NOT") == 0) {
        Type res = Exp(root->children[1]);
        if (res != NULL)
            if (res->kind != ENUM_BASIC || res->basic != INT_TYPE) {
                printf("Error type 7 at line %d: Operands type mismatched.\n", root->line_number);
                return NULL;
            }
        return res;
    }
    else if (strcmp(root->children[0]->node_name, "ID") == 0) {
        // ID是一个变量
        if (root->num_children == 1) {
            Entry sym = findSymbolAll(root->children[0]->str_value);
            // 使用不存在的变量
            if (sym == NULL) {
                printf("Error type 1 at line %d: Undefined variable \"%s\".\n", root->line_number, root->children[0]->str_value);
                return NULL;
            }
            return sym->type;
        }
        // ID是一个函数名
        else {
            Entry sym = findSymbolFunc(root->children[0]->str_value);
            if (sym == NULL) {
                sym = findSymbolAll(root->children[0]->str_value);
                // 对普通变量使用()操作符
                if (sym != NULL) {
                    printf("Error type 11 at line %d: \"%s\" is not a function.\n", root->line_number, sym->node_name);
                    return NULL;
                }
                // 使用不存在的函数
                else {
                    printf("Error type 2 at line %d: Undefined function \"%s\".\n", root->line_number, root->children[0]->str_value);
                    return NULL;
                }
            }
            // 使用未定义的函数
            if (sym->type->func->hasDefined == 0) {
                printf("Error type 2 at line %d: Undefined function \"%s\".\n", root->line_number, sym->node_name);
                return NULL;
            }
            // 提供的参数
            FieldList args = NULL;
            FieldList args_ = NULL;
            // 函数应有的参数
            FieldList realArgs = sym->type->func->head;
            // 函数有参数
            if (root->num_children == 4) {
                args = Args(root->children[2]);
                args_ = args;
            }
            int flag = 1;
            while (args != NULL && realArgs != NULL) {
                if (!typeEqual(args->type, realArgs->type)) {
                    flag = 0;
                    break;
                }
                args = args->next;
                realArgs = realArgs->next;
            }
            if (args != NULL || realArgs != NULL)
                flag = 0;
            if (flag == 0) {
                printf("Error type 9 at line %d: The method \"%s(", root->line_number, sym->node_name);
				printArgs(sym->type->func->head);
				printf(")\" is not applicable for the arguments \"(");
                printArgs(args_);
                printf(")\".\n");
            }
            return sym->type->func->returnType;
        }
    }
    else if (strcmp(root->children[0]->node_name, "INT") == 0) {
        Type res = (Type)malloc(sizeof(Type_));
        res->kind = ENUM_BASIC;
        res->basic = INT_TYPE;
        return res;
    }
    else if (strcmp(root->children[0]->node_name, "FLOAT") == 0) {
        Type res = (Type)malloc(sizeof(Type_));
        res->kind = ENUM_BASIC;
        res->basic = FLOAT_TYPE;
        return res;
    }
}

// 处理函数调用时的参数列表
FieldList Args(TreeNode* root) {
    FieldList res = (FieldList)malloc(sizeof(FieldList_));  // 创建一个 FieldList 结构体
    res->type = Exp(root->children[0]);                     // 处理第一个参数表达式，并将其类型赋给 res
    if (root->num_children == 3)
        res->next = Args(root->children[2]);                // 递归处理下一个参数，并将其添加到 res 的 next 字段
    return res;                                             // 返回参数列表
}

// 打印参数类型列表
void printArgs(FieldList head) {
    if (head == NULL)
        return;
    if (head->next == NULL) {
        printType(head->type);
        return;
    }
    printType(head->type);
    printf(", ");
    printArgs(head->next);
}

// 打印类型
void printType(Type type) {
	if (type->kind == ENUM_BASIC) {
        if (type->basic == INT_TYPE)
            printf("int");
        else
            printf("float");
    }
	else if (type->kind == ENUM_STRUCT)
		printf("struct %s", type->structure->node_name);
	else if (type->kind == ENUM_ARRAY) {
		printType(type->array.elem);
		printf("[]");
	}
    else if (type->kind = ENUM_FUNC)
        printf("func %s", type->func->node_name);
    else if (type->kind == ENUM_STRUCT_DEF)
        printf("struct definition %s", type->structure->node_name);
}