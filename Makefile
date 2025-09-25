# 示例Makefile文件
# 包含多个目标和依赖关系，用于测试makefile解析器

hello.o:hello.c
	gcc -c hello.c 

main.o: main.c 
	gcc -c main.c -o main.o

program:main.o hello.o
	gcc main.o hello.o -o program













