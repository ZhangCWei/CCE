%option yylineno
%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include "ParseTree.h"
    #define YYSTYPE TreeNode*
    #include "syntax.tab.h"

    int yycol = 1;
    int lexicalErrorCount = 0;
    int ToDec(char c);
    int HexTo(char* c);
    int OctTo(char* c);

    // 定义token
    extern YYSTYPE Token;

    // 设置列信息，用于报错
    #define YY_USER_ACTION \
        yylloc.first_line = yylineno; \
        yylloc.last_line = yylineno; \
        yylloc.first_column = yycol; \
        yylloc.last_column = (yycol += yyleng) - 1;
%}

WhSpace   [ \t\r]+
digit     [0-9]
letter    [A-Za-z_]
DEC_NUM   0|[1-9]{digit}*
OCT_NUM   0[0-9]+
HEX_NUM   0[xX][0-9a-zA-Z]+
INT       {DEC_NUM}|{OCT_NUM}|{HEX_NUM}
FLOAT     (({digit}*\.{digit}+|{digit}+\.)[eE][+-]?{digit}+)|({digit}+\.{digit}+)
ID        {letter}+({letter}|{digit})*
RELOP     ">"|"<"|">="|"<="|"=="|"!="
TYPE      "int"|"float"

%%
"\n"            { yycol = 1; }
{WhSpace}       { /* Do nothing */ }
"//"            { /* 匹配单行注释 */
                 char next;
                 do {
                     next = input();
                 } while (next != '\n');
                }
"/*"            { /* 匹配多行注释的起始符号 */
                 char next = input();
                 while (1) {
                 if (next == '*') {
                    char next_next = input();
                    if (next_next == '/')  break;
                 }else if (next == '\0') {
                        printf("Error type B at Line %d: Incomplete annotation.\n", yylineno);
                        exit(EXIT_FAILURE);
                    }
                    next = input();
                 }
                }

";"             { yylval = createTreeNode("SEMI", NODE_TYPE_OTHER, yylineno, 0, NULL); return SEMI; }
","             { yylval = createTreeNode("COMMA", NODE_TYPE_OTHER, yylineno, 0, NULL); return COMMA; }
"("             { yylval = createTreeNode("LP", NODE_TYPE_OTHER, yylineno, 0, NULL); return LP; }
")"             { yylval = createTreeNode("RP", NODE_TYPE_OTHER, yylineno, 0, NULL); return RP; }
"["             { yylval = createTreeNode("LB", NODE_TYPE_OTHER, yylineno, 0, NULL); return LB; }
"]"             { yylval = createTreeNode("RB", NODE_TYPE_OTHER, yylineno, 0, NULL); return RB; }
"{"             { yylval = createTreeNode("LC", NODE_TYPE_OTHER, yylineno, 0, NULL); return LC; }
"}"             { yylval = createTreeNode("RC", NODE_TYPE_OTHER, yylineno, 0, NULL); return RC; }
"="             { yylval = createTreeNode("ASSIGNOP", NODE_TYPE_OTHER, yylineno, 0, NULL); return ASSIGNOP; }
"+"             { yylval = createTreeNode("PLUS", NODE_TYPE_OTHER, yylineno, 0, NULL); return PLUS; }
"-"             { yylval = createTreeNode("MINUS", NODE_TYPE_OTHER, yylineno, 0, NULL); return MINUS; }
"*"             { yylval = createTreeNode("STAR", NODE_TYPE_OTHER, yylineno, 0, NULL); return STAR; }
"/"             { yylval = createTreeNode("DIV", NODE_TYPE_OTHER, yylineno, 0, NULL); return DIV; }
"&&"            { yylval = createTreeNode("AND", NODE_TYPE_OTHER, yylineno, 0, NULL); return AND; }
"||"            { yylval = createTreeNode("OR", NODE_TYPE_OTHER, yylineno, 0, NULL); return OR; }
"."             { yylval = createTreeNode("DOT", NODE_TYPE_OTHER, yylineno, 0, NULL); return DOT; }
"!"             { yylval = createTreeNode("NOT", NODE_TYPE_OTHER, yylineno, 0, NULL); return NOT; }

"struct"        { yylval = createTreeNode("STRUCT", NODE_TYPE_OTHER, yylineno, 0, NULL); return STRUCT; }
"if"            { yylval = createTreeNode("IF", NODE_TYPE_OTHER, yylineno, 0, NULL); return IF; }
"else"          { yylval = createTreeNode("ELSE", NODE_TYPE_OTHER, yylineno, 0, NULL); return ELSE; }
"while"         { yylval = createTreeNode("WHILE", NODE_TYPE_OTHER, yylineno, 0, NULL); return WHILE; }
"return"        { yylval = createTreeNode("RETURN", NODE_TYPE_OTHER, yylineno, 0, NULL); return RETURN; }
{RELOP}         { yylval = createTreeNode("RELOP", NODE_TYPE_OTHER, yylineno, 0, NULL); return RELOP; }

{TYPE}          { yylval = createTreeNode("TYPE", NODE_TYPE_TYPE, yylineno, 0, NULL); strcpy(yylval->str_value, yytext); return TYPE; }
{HEX_NUM}       { 
                    int hexValue = HexTo(yytext);
                    if (hexValue == -1) {
                        printf("Error type A at Line %d: Illegal hexadecimal number \'%s\' \n", yylineno, yytext); 
                        lexicalErrorCount++;
                    } else {
                        yylval = createTreeNode("INT", NODE_TYPE_INTEGER, yylineno, 0, NULL); 
                        yylval->int_value = hexValue; 
                        return INT; 
                    }
                }
{OCT_NUM}       { 
                    int octValue = OctTo(yytext);
                    if (octValue == -1) {
                        printf("Error type A at Line %d: Illegal octal number \'%s\' \n", yylineno, yytext); 
                        lexicalErrorCount++;
                    } else {
                        yylval = createTreeNode("INT", NODE_TYPE_INTEGER, yylineno, 0, NULL); 
                        yylval->int_value = octValue; 
                        return INT; 
                    }
                }
{DEC_NUM}       { yylval = createTreeNode("INT", NODE_TYPE_INTEGER, yylineno, 0, NULL); yylval->int_value = atoi(yytext); return INT; }
{FLOAT}         { yylval = createTreeNode("FLOAT", NODE_TYPE_FLOAT, yylineno, 0, NULL); yylval->float_value = atof(yytext); return FLOAT; }

{ID}            { yylval = createTreeNode("ID", NODE_TYPE_IDENTIFIER, yylineno, 0, NULL); strcpy(yylval->str_value, yytext); return ID; }

.               { printf("Error type A at Line %d: Invalid character \'%s\' \n", yylineno, yytext); lexicalErrorCount++; }

%%

int ToDec(char c) {
    if (c >= '0' && c <= '7') {
        return c - '0';
    } else {
        if (isalpha(c)) {
            if (c >= 'A' && c <= 'F') return c - 'A';
            else if (c >= 'a' && c <= 'f') return c - 'a';
        }
    }
    return -1;
}

int HexTo(char* c) {
    int result = 0, i = 2;
    while (c[i] != '\0') {
        if (isxdigit(c[i])) {
            int digit = 0;
            if (isdigit(c[i])) {
                digit = c[i] - '0';
            } else {
                digit = toupper(c[i]) - 'A' + 10;
            }
            result = result * 16 + digit;
            i++;
        } else {
            return -1;
        }
    }
    return result;
}

int OctTo(char* c) {
    int result = 0, i = 1;
    while (c[i] != '\0') {
        if (c[i] >= '0' && c[i] <= '7') {
            int digit = c[i] - '0';
            result = result * 8 + digit;
            i++;
        } else {
            return -1;
        }
    }
    return result;
}
