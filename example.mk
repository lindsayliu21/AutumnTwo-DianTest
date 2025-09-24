# 示例Makefile文件
# 包含多个目标和依赖关系，用于测试makefile解析器

main.o: main.c defs.h
	gcc -c main.c

defs.h: parse.c
	touch defs.h

parse.o: parse.c defs.h
	gcc -c parse.c

lexer.o: lexer.c defs.h
	gcc -c lexer.c

util.o: util.c
	gcc -c util.c

app: main.o parse.o lexer.o util.o
	gcc -o app main.o parse.o lexer.o util.o