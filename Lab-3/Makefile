# 编译器设定和编译选项
CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -std=c99

# 编译目标：src目录下的所有.c文件
CFILES = $(shell find ./ -name "*.c")
OBJS = $(CFILES:.c=.o)
LFILE = $(shell find ./ -name "*.l")
YFILE = $(shell find ./ -name "*.y")
LFC = $(shell find ./ -name "*.l" | sed s/[^/]*\\.l/lex.yy.c/)
YFC = $(shell find ./ -name "*.y" | sed s/[^/]*\\.y/syntax.tab.c/)
LFO = $(LFC:.c=.o)
YFO = $(YFC:.c=.o)

parser: syntax $(filter-out $(LFO),$(OBJS))
	$(CC) -o parser $(filter-out $(LFO),$(OBJS)) -lfl 

syntax: lexical syntax-c
	$(CC) -c $(YFC) -o $(YFO)

lexical: $(LFILE)
	$(FLEX) -o $(LFC) $(LFILE)

syntax-c: $(YFILE)
	$(BISON) -o $(YFC) -d -v $(YFILE)

-include $(patsubst %.o, %.d, $(OBJS))

parser_:
	flex -o ./lex.yy.c ./lexical.l
	bison -o ./syntax.tab.c -d -v ./syntax.y
	gcc -g -c ./syntax.tab.c -o ./syntax.tab.o
	gcc -std=c99 -g -c -o intercode.o intercode.c
	gcc -std=c99 -g -c -o semantic.o semantic.c
	gcc -std=c99 -g -c -o Tree.o Tree.c
	gcc -std=c99 -g -c -o main.o main.c
	gcc -g -o parser ./intercode.o ./semantic.o ./Tree.o ./syntax.tab.o ./main.o -lfl


TEST_FILES = $(wildcard ./Test/*.cmm)
IR_FILES = $(patsubst ./Test/%.cmm, ./Result/%.ir, $(TEST_FILES))

# 生成所有测试文件的IR结果
all: $(IR_FILES)

# 生成单个测试文件的IR结果
./Result/%.ir: ./Test/%.cmm parser_
	./parser $< $@

# 定义的一些伪目标
.PHONY: clean test all
test: parser_
	./parser ./Test/test_d1.cmm ./Result/out.ir

clean:
	rm -f parser lex.yy.c syntax.tab.c syntax.tab.h syntax.output
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f $(LFC) $(YFC) $(YFC:.c=.h)
	rm -f *~
