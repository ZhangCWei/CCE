编译源代码的指令依次如下： 

flex lexical.l

bison -d -v syntax.y

gcc main.c syntax.tab.c ParseTree.c -lfl -o parser

按顺序编译后， 即可生成相应的 parser 文件。

