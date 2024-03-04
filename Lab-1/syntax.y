%locations
%define parse.error verbose

%{
    #include <stdarg.h>
    #include "lex.yy.c"
    void yyerror(const char* msg);
    int syntaxErrorCount = 0;
    TreeNode* root = NULL;
    TreeNode** createNodeList(int numNodes, TreeNode* firstNode, ...);
%}

%token INT FLOAT ID SEMI COMMA ASSIGNOP RELOP 
%token PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
/* High-level Definitions */
Program : ExtDefList                            { $$ = createTreeNode("Program", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  1, createNodeList(1, $1));
                                                  root = $$; }
    ;
ExtDefList : ExtDef ExtDefList                  { $$ = createTreeNode("ExtDefList", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  2, createNodeList(2, $1, $2)); }
    | /* empty */                               { $$ = createTreeNode("ExtDefList", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); }
    ;
ExtDef : Specifier ExtDecList SEMI              { $$ = createTreeNode("ExtDef", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  3, createNodeList(3, $1, $2, $3)); }
    | Specifier SEMI                            { $$ = createTreeNode("ExtDef", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  2, createNodeList(2, $1, $2)); }
    | Specifier FunDec CompSt                   { $$ = createTreeNode("ExtDef", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  3, createNodeList(3, $1, $2, $3)); }
    | Specifier error SEMI                      { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    | error SEMI                                { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    | Specifier error                           { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    ;
ExtDecList : VarDec                             { $$ = createTreeNode("ExtDecList", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  1, createNodeList(1, $1)); }
    | VarDec COMMA ExtDecList                   { $$ = createTreeNode("ExtDecList", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  3, createNodeList(3, $1, $2, $3)); }
    | VarDec error COMMA ExtDecList             { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    ;

/* Specifiers */
Specifier : TYPE                                { $$ = createTreeNode("Specifier", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  1, createNodeList(1, $1)); }
    | StructSpecifier                           { $$ = createTreeNode("Specifier", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  1, createNodeList(1, $1)); }
    ;
StructSpecifier : STRUCT OptTag LC DefList RC   { $$ = createTreeNode("StructSpecifier", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  5, createNodeList(5, $1, $2, $3, $4, $5)); }
    | STRUCT Tag                                { $$ = createTreeNode("StructSpecifier", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  2, createNodeList(2, $1, $2)); }
    | STRUCT error LC DefList RC                { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    | STRUCT OptTag LC error RC                 { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    | STRUCT OptTag LC error                    { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    | STRUCT error                              { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    ;
OptTag : ID                                     { $$ = createTreeNode("OptTag", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  1, createNodeList(1, $1)); }
    | /* empty */                               { $$ = createTreeNode("OptTag", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); }
    ;
Tag : ID                                        { $$ = createTreeNode("Tag", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  1, createNodeList(1, $1)); }
    ;

/* Declarators */
VarDec : ID                                     { $$ = createTreeNode("VarDec", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  1, createNodeList(1, $1)); }
    | VarDec LB INT RB                          { $$ = createTreeNode("VarDec", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  4, createNodeList(4, $1, $2, $3, $4)); }
    | VarDec LB error RB                        { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    | VarDec LB error                           { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    ;
FunDec : ID LP VarList RP                       { $$ = createTreeNode("FunDec", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  4, createNodeList(4, $1, $2, $3, $4)); }
    | ID LP RP 					                { $$ = createTreeNode("FunDec", NODE_TYPE_NOT_NULL, @$.first_line,
    						                      3, createNodeList(3, $1, $2, $3)); }
    | ID LP error RP                            { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    | ID LP error                               { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    ;
VarList : ParamDec COMMA VarList                { $$ = createTreeNode("VarList", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  3, createNodeList(3, $1, $2, $3)); }
    | ParamDec                                  { $$ = createTreeNode("VarList", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  1, createNodeList(1, $1)); }
    ;
ParamDec : Specifier VarDec                     { $$ = createTreeNode("ParamDec", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  2, createNodeList(2, $1, $2)); }
    ;

/* Statements */
CompSt : LC DefList StmtList RC                 { $$ = createTreeNode("CompSt", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  4, createNodeList(4, $1, $2, $3, $4)); }
    ;
StmtList : Stmt StmtList                        { $$ = createTreeNode("StmtList", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  2, createNodeList(2, $1, $2)); }
    | /* empty */                               { $$ = createTreeNode("StmtList", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); }
    ;
Stmt : Exp SEMI                                 { $$ = createTreeNode("Stmt", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  2, createNodeList(2, $1, $2)); }
    | CompSt                                    { $$ = createTreeNode("Stmt", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  1, createNodeList(1, $1)); }
    | RETURN Exp SEMI                           { $$ = createTreeNode("Stmt", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  3, createNodeList(3, $1, $2, $3)); }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE   { $$ = createTreeNode("Stmt", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  5, createNodeList(5, $1, $2, $3, $4, $5)); }
    | IF LP Exp RP Stmt ELSE Stmt               { $$ = createTreeNode("Stmt", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  7, createNodeList(7, $1, $2, $3, $4, $5, $6, $7)); }
    | WHILE LP Exp RP Stmt                      { $$ = createTreeNode("Stmt", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  5, createNodeList(5, $1, $2, $3, $4, $5)); }
    | error SEMI                                { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    | IF LP error RP Stmt %prec LOWER_THAN_ELSE { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    | IF LP Exp RP error ELSE Stmt              { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line, 
                                                  0, NULL); yyerrok; }
    | IF LP error RP ELSE Stmt                  { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line, 
                                                  0, NULL); yyerrok; }
    | error LP Exp RP Stmt                      { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line, 
                                                  0, NULL); yyerrok; }
    ;


/* Local Definitions */
DefList : Def DefList                           { $$ = createTreeNode("DefList", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  2, createNodeList(2, $1, $2)); }
    | /* empty */                               { $$ = createTreeNode("Stmt", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); }
    ;
Def : Specifier DecList SEMI                    { $$ = createTreeNode("Def", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  3, createNodeList(3, $1, $2, $3)); }
    ;
DecList : Dec                                   { $$ = createTreeNode("DecList", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  1, createNodeList(1, $1)); }
    | Dec COMMA DecList                         { $$ = createTreeNode("DecList", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  3, createNodeList(3, $1, $2, $3)); }
    | Dec error DecList                         { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    ;
Dec : VarDec                                    { $$ = createTreeNode("Dec", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  1, createNodeList(1, $1)); }
    | VarDec ASSIGNOP Exp                       { $$ = createTreeNode("Dec", NODE_TYPE_NOT_NULL, @$.first_line,
                                                  3, createNodeList(3, $1, $2, $3)); }
    | error ASSIGNOP Exp                        { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line,
                                                  0, NULL); yyerrok; }
    ;

/* Expressions */
Exp : Exp ASSIGNOP Exp                          { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 3, createNodeList(3, $1, $2, $3)); }
    | Exp AND Exp                               { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 3, createNodeList(3, $1, $2, $3)); }
    | Exp OR Exp                                { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 3, createNodeList(3, $1, $2, $3)); }
    | Exp RELOP Exp                             { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 3, createNodeList(3, $1, $2, $3)); }
    | Exp PLUS Exp                              { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 3, createNodeList(3, $1, $2, $3)); }
    | Exp MINUS Exp                             { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 3, createNodeList(3, $1, $2, $3)); }
    | Exp STAR Exp                              { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 3, createNodeList(3, $1, $2, $3)); }
    | Exp DIV Exp                               { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 3, createNodeList(3, $1, $2, $3)); }
    | LP Exp RP                                 { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 3, createNodeList(3, $1, $2, $3)); }
    | MINUS Exp                                 { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 2, createNodeList(2, $1, $2)); }
    | NOT Exp                                   { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 2, createNodeList(2, $1, $2)); }
    | ID LP Args RP                             { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 4, createNodeList(4, $1, $2, $3, $4)); }
    | ID LP RP                                  { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 3, createNodeList(3, $1, $2, $3)); }
    | Exp LB Exp RB                             { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 4, createNodeList(4, $1, $2, $3, $4)); }
    | Exp DOT ID                                { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 3, createNodeList(3, $1, $2, $3)); }
    | ID                                        { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 1, createNodeList(1, $1)); }
    | INT                                       { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 1, createNodeList(1, $1)); }
    | FLOAT                                     { $$ = createTreeNode("Exp", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 1, createNodeList(1, $1)); }
    | Exp ASSIGNOP error                        { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line
                                                  , 0, NULL); yyerrok; }
    | Exp AND error                             { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line
                                                  , 0, NULL); yyerrok; }
    | Exp OR error                              { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line
                                                  , 0, NULL); yyerrok; }
    | Exp RELOP error                           { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line
                                                  , 0, NULL); yyerrok; }
    | Exp PLUS error                            { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line
                                                  , 0, NULL); yyerrok; }
    | Exp MINUS error                           { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line
                                                  , 0, NULL); yyerrok; }
    | Exp STAR error                            { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line
                                                  , 0, NULL); yyerrok; }
    | Exp DIV error                             { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line
                                                  , 0, NULL); yyerrok; }
    | ID LP error RP                            { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line
                                                  , 0, NULL); yyerrok; }
    | Exp LB error RB                           { $$ = createTreeNode("Error", NODE_TYPE_NULL, @$.first_line
                                                  , 0, NULL); yyerrok; }
    ;
Args : Exp COMMA Args                           { $$ = createTreeNode("Args", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 3, createNodeList(3, $1, $2, $3)); }
    | Exp                                       { $$ = createTreeNode("Args", NODE_TYPE_NOT_NULL, @$.first_line
                                                  , 1, createNodeList(1, $1)); }
    ;
%%
TreeNode** createNodeList(int numNodes, TreeNode* firstNode, ...) {
    va_list l;
    va_start(l, firstNode);
    TreeNode** result = (TreeNode**)malloc(sizeof(TreeNode*) * numNodes);
    for (int i = 0; i < numNodes; i++) {
        result[i] = (i == 0) ? firstNode : va_arg(l, TreeNode*);
    }
    return result;
}

void yyerror(const char* msg) { 
    syntaxErrorCount++;
    printf("Error type B at Line %d: %s\n", yylineno, msg);
}
