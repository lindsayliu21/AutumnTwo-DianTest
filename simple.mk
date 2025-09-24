# 简单的Makefile示例
# 用于基本功能测试

hello.o: hello.c
	gcc -c hello.c

main.o: main.c
	gcc -c main.c -o main.o	

program.o: hello.o main.o
	gcc -o program hello.o main.o

app:program.o
	gcc -o app program.o